#!/usr/bin/env python3
"""
Track-2 runner (investigative MCQ loop), per WAITI-paper/benchmark_execution_design.md section 4 and the
loop agreed with benchmark-v2 on 2026-09-25 (session_buffer.md 17:42).

Per unit (one model, one seed):
  R0  show the start body, ask for LABEL (same prompt as Track-1).
      vulnerable / not-vulnerable at R0 -> early commit (flagged, not credited as a clean resolution).
  If need-more-context: show a numbered menu (order shuffled with a fixed per-unit seed). The model replies
  with ONE line: "PICK: <n>" or "LABEL: vulnerable|not-vulnerable".
      on-target pick, not yet delivered -> the artifact is delivered, menu shown again.
      re-pick of a delivered item      -> refused (counts against request precision), round used.
      off-target pick                   -> unit ends, FAIL.
      LABEL v/nv                         -> unit ends, graded against the terminal.
  Round cap = number of on-target items + 2. Hitting the cap without a commit = FAIL.
Only final answers are kept in the chat history (reasoning is dropped between rounds).

Unit data comes from the "track2" block in the unit's slices.json:
  terminal, start_slice, context_menu[{key, kind: on-target|off-target, artifact, label?, deliver?}]
The model only ever sees "label" (neutral text written by the data owner). Until labels exist, a neutral
fallback is derived from the artifact (address / symbol only, no description); such units are marked
label_fallback=true. "deliver" is a path relative to the unit dir or a literal text; when missing, the runner
looks for a body file whose name holds the artifact address (unit dir, ctx/, staged cve_ctx/<unit>/ctx/).
Undeliverable on-target items are recorded as deliver_missing and the model is told the item is unavailable.

Usage:
  python3 run_track2.py --port 8102 --model gptoss20b --units CVE-2023-1327__RAX30_V1.0.3.64 ... \
        --run-name gpt-oss-20b__s1 --seed 1 [--reasoning default] [--out results_t2/x.jsonl]
  (--index slices/_track2/track2_index.json instead of --units, once the index is final)
"""
import os, sys, json, re, glob, random, hashlib, time, argparse, threading, urllib.error
from concurrent.futures import ThreadPoolExecutor, as_completed

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from run_track1 import USR_SUFFIX, post, final_text, parse_label, prompt_tokens, chat  # same answer format + client

# Track-2 scope (user decision 2026-09-25): the bug class is a privileged operation reachable by the wrong or an
# unauthenticated party. Pre-authentication command injection is in scope as that intersection (running a shell
# command is the privileged operation); post-authentication command injection is not.
SYS_P = ("You are auditing one decompiled firmware function for an authentication or authorization logic weakness: "
         "a privileged operation that a wrong or unauthenticated party can reach. This includes pre-authentication "
         "command injection (attacker input reaching a shell command with no authentication on the path). "
         "Command injection that requires a valid login is out of scope. Decide from the evidence you have.")
import build_instances as _bi
from build_instances import slice_body, _va_keys
# T2_BASE: a frozen copy of the benchmark root (k1k2_localization) - every file a unit reads then comes from the copy.
BASE = os.environ.get("T2_BASE", _bi.BASE)
_bi.BASE = BASE; _bi.CVE_CTX = f"{BASE}/slices/_incoming/cve_ctx"   # slice_body() staged lookup follows the copy too

SLICES = f"{BASE}/slices"
STAGED = f"{SLICES}/_incoming/cve_ctx"
T2STAGE = os.environ.get("T2_STAGE_DIR", f"{SLICES}/_incoming/_track2")   # staged track2_block.json (+ ctx/) per unit
# T2_PREFER_STAGE=1: read blocks/flip files from the (snapshot) stage dirs BEFORE canonical paths, so a fold that lands
# mid-run cannot change the data of a running experiment.
PREFER_STAGE = os.environ.get("T2_PREFER_STAGE") == "1"
PICK = re.compile(r"PICK:\s*[<\[(\"'`]*\s*(\d+)", re.I)
COMMIT = re.compile(r"LABEL:\s*[<\[*\"'`]*\s*(need-more-context|not-vulnerable|vulnerable|undecidable)\b", re.I)
UNDEC = re.compile(r"\bUNDECIDABLE\b", re.I)
RESHOW_LABEL = "show this same function body again"

# Menu wording locked with benchmark-v2 and the data owner (session_buffer.md 2026-09-25 19:55): state the stakes.
MENU_HEAD = "Context menu:\n"
# Moves agreed with the user 2026-09-25: PICK (want more), LABEL v/nv (decide), UNDECIDABLE (give up, final).
MENU_TAIL = ("\n\nYou may request ONE item from the menu with 'PICK: <number>'.\n"
             "Give 'LABEL: vulnerable' or 'LABEL: not-vulnerable' only when you are ready to decide; that ends the "
             "investigation. Write 'UNDECIDABLE' if you believe the case cannot be decided with the evidence "
             "available; that also ends it.\n"
             "Warning: some menu items are not part of this case; picking one ends the investigation immediately and "
             "cannot be undone. There is no backtracking.\n"
             "A LABEL here is your final verdict on the whole case, using ALL evidence shown so far (not only the first "
             "body): vulnerable = the evidence shows a privileged operation reachable by a wrong or unauthenticated "
             "party; not-vulnerable = the evidence shows it is protected, or attacker input does not reach the operation.\n"
             "Reply with exactly one line: 'PICK: <number>', 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.")
# Closed-world ablation (follow-up run only, --closed-world): the model is told the menu is sufficient.
CLOSED_WORLD = ("The context menu for this case contains everything needed to decide it: the items that belong to the case "
                "are sufficient to reach a verdict.")
NUDGE = ("need-more-context is not a final answer at this stage. To get more evidence, PICK an item from the menu; "
         "otherwise give 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.\n\n")
FINAL_ALL = ("You now have all the evidence available for this case. Give your final verdict in exactly one line: "
             "'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.")
FINAL_CAP = ("The investigation has reached its round limit. Give your final verdict in exactly one line: "
             "'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.")
FORCE_T2 = "Reply with one line only: 'PICK: <number>', 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'."

# ---------- unit loading ----------
def unit_dir(name):
    for p in [f"{SLICES}/cve/{name}", f"{SLICES}/big-context/{name}"] + glob.glob(f"{SLICES}/non-cves/*/{name}"):
        if os.path.isdir(p): return p
    raise FileNotFoundError(name)

def neutral_label(artifact):
    """Fallback menu text: keep only the address / symbol, drop every description (they leak the answer)."""
    a = str(artifact).strip()
    core = re.split(r"\s[\(\-]|\s\(|\s->|;", a)[0].strip()
    if core.startswith("*"): return f"the value stored at {core}"
    if re.fullmatch(r"0x[0-9a-fA-F]+", core): return f"body of function {core}"
    if "(" in core and "@" in a:  # e.g. cgiFormString(cmd) @0x1261c
        return f"origin of the input read by {core}"
    return f"body of {core}"

def find_body_file(udir, unit, artifact):
    keys = _va_keys({"va": " ".join(re.findall(r"0x[0-9a-fA-F]+", str(artifact)))})
    for h in keys:
        for pat in (f"{udir}/*0x{h}*.c", f"{udir}/*{h}*.c", f"{udir}/ctx/*{h}*.c", f"{STAGED}/{unit}/ctx/*{h}*.c"):
            for p in sorted(glob.glob(pat)):
                if os.path.getsize(p) > 0: return p
    return None

def start_body(udir, start, extra_dirs=()):
    """Start body: the slice at the start address (slices.json), else fn.c (hardneg shape), else a body file named
    with the start address in the staged dirs (_incoming/_track2/<unit>, _incoming/cve_ctx/<unit>, and their ctx/)."""
    sj = f"{udir}/slices.json"
    if os.path.exists(sj):
        d = json.load(open(sj))
        sl = next((s for s in d.get("slices", []) if set(_va_keys(s)) & set(_va_keys({"va": start}))), {"va": start})
        b = slice_body(sl, udir)
        if b: return b
    for p in (f"{udir}/fn.c",):
        if os.path.exists(p) and os.path.getsize(p) > 0: return open(p).read()
    b = slice_body({"va": start}, udir)
    if b: return b
    name = os.path.basename(udir.rstrip("/"))
    for d in list(extra_dirs) + [f"{T2STAGE}/{name}", f"{STAGED}/{name}"]:
        for h in _va_keys({"va": start}):
            for pat in (f"{d}/*{h}*.c", f"{d}/ctx/*{h}*.c"):
                for p in sorted(glob.glob(pat)):
                    if os.path.getsize(p) > 0: return open(p).read()
    return None

SNAP_ROOT = os.environ.get("T2_SNAP_ROOT")  # snapshot copy of BASE-relative files (e.g. slices/non-cves/synthetic/...)
PATHLIKE = re.compile(r"[\w./+-]+\.(c|h|txt|json|php|sh|conf|xml|js|asp|htm|html)")

def resolve_file(rel, dirs=()):
    """Find a data file given relative to a unit/group dir OR to the benchmark root (BASE). Snapshot root first."""
    if not rel: return None
    cands = [os.path.join(d, rel) for d in dirs]
    if SNAP_ROOT: cands.append(os.path.join(SNAP_ROOT, rel))
    cands += [os.path.join(BASE, rel), os.path.join(SLICES, rel), rel]
    return next((c for c in cands if os.path.isfile(c)), None)

STRIP = os.environ.get("STRIP_COMMENTS") == "1"   # 2026-09-27 re-run: decompiled code without comments
if STRIP:
    from textclean import strip_comments as _strip, strip_comments_ext as _strip_ext
else:
    _strip = lambda t: t
    _strip_ext = lambda t, p: t   # path-aware strip (.c/.h/.php/.js; # only for .php); identity when STRIP off

def build_unit(name, t2, src_dirs, body, udir=None, extra=None):
    body = _strip(body)
    items, fallback = [], False
    for it in t2.get("context_menu", []):
        lab = it.get("label")
        if not lab: lab = neutral_label(it.get("artifact", it.get("key"))); fallback = True
        content, src, kind = None, None, it.get("kind")
        dv = it.get("deliver")
        if kind == "reshow":
            content, src = body, "<same body>"
        elif kind == "on-target":
            path = resolve_file(dv, src_dirs) if dv else None
            if path:  # strip comments from code sources (.c/.h/.php/.js); prose notes (.txt/.md/.sh) cleaned in data
                raw = open(path).read()
                content = _strip_ext(raw, path)
                src = os.path.relpath(path, SLICES)
            elif dv and PATHLIKE.fullmatch(str(dv).strip()):  # a file path that does not resolve is a data error,
                raise FileNotFoundError(f"{name}: deliver path not found: {dv}")  # never show the path as content
            elif dv: content, src = str(dv), "literal"
            elif udir:
                p = find_body_file(udir, name, it.get("artifact", ""))
                if p: content, src = _strip_ext(open(p).read(), p), os.path.relpath(p, SLICES)
        items.append({"key": it.get("key"), "kind": kind, "label": lab, "content": content, "src": src})
    if not any(i["kind"] == "reshow" or i["label"] == RESHOW_LABEL for i in items):
        items.append({"key": "reshow_start", "kind": "off-target", "label": RESHOW_LABEL, "content": None, "src": None})
    seed_name = (extra or {}).get("group") or name   # flip group members share ONE menu order
    rnd = random.Random(int(hashlib.md5(seed_name.encode()).hexdigest()[:8], 16))  # fixed per-unit order
    rnd.shuffle(items)
    u = {"unit": name, "terminal": str(t2.get("terminal")), "start": t2.get("start_slice"), "body": body,
         "items": items, "n_on": sum(i["kind"] == "on-target" for i in items), "label_fallback": fallback,
         "source": os.path.relpath(src_dirs[0], SLICES),
         "deliver_missing": [i["key"] for i in items if i["kind"] == "on-target" and not i["content"]]}
    u.update(extra or {})
    return u

def load_unit(name, staged=False, block=None):
    """Core unit. block = path to track2_block.json (index entry); else staged dir; else slices.json track2."""
    udir = unit_dir(name)
    sdir = f"{T2STAGE}/{name}"
    canon = [os.path.join(BASE, block), os.path.join(SLICES, block)] if block else []
    stage = [f"{sdir}/track2_block.json"] if staged or block else []
    cands = stage + canon if PREFER_STAGE else canon + stage
    bpath = next((c for c in cands if c and os.path.exists(c)), None)
    if bpath:
        t2 = json.load(open(bpath)); src_dirs = [os.path.dirname(bpath), udir]
    else:
        if block: raise FileNotFoundError(f"track2 block for {name}: {block}")
        t2 = json.load(open(f"{udir}/slices.json")).get("track2") or {}; src_dirs = [udir]
    sb = t2.get("start_body")  # explicit path (relative to the block dir, the unit dir, or BASE) wins
    sp = resolve_file(sb, src_dirs) if sb else None
    if sb and not sp: raise FileNotFoundError(f"{name}: start_body not found: {sb}")
    body = open(sp).read() if sp else start_body(udir, t2.get("start_slice"), src_dirs[:1])
    return build_unit(name, t2, src_dirs, body, udir)

def load_flip(path):
    """Synthetic flip group: one shared menu, an original (expected not-vulnerable) and mutants (expected
    vulnerable). Returns one unit per body; all members share the menu and its order."""
    d = json.load(open(path)); gdir = os.path.dirname(path); gate = d.get("gate") or os.path.basename(gdir)
    menu = dict(d["menu"])
    members = [("original", "original", d["original"])] + [(m.get("id") or f"M{i}", "mutant", m) for i, m in enumerate(d["mutants"], 1)]
    out = []
    for mid, role, m in members:
        bp = resolve_file(m["body"], [gdir])
        if not bp: raise FileNotFoundError(f"flip {gate}/{mid}: body file not found: {m['body']}")
        body = open(bp).read()
        t2 = {**menu, "terminal": m.get("expected"), "start_slice": d.get("source_va")}
        out.append(build_unit(f"flip/{gate}/{mid}", t2, [gdir], body, None,
                              {"group": gate, "role": role, "mutation": m.get("mutation_name")}))
    return out

T2SYNTH = os.environ.get("T2_SYNTH_DIR", f"{SLICES}/_incoming/_track2_synth")

def _resolve(path, fallback):
    """Index paths point at the folded (canonical) location; before the fold the file lives under _incoming/."""
    order = (fallback, os.path.join(BASE, path), os.path.join(SLICES, path), path) if PREFER_STAGE else \
            (os.path.join(BASE, path), os.path.join(SLICES, path), path, fallback)
    for c in order:
        if c and os.path.exists(c): return c
    raise FileNotFoundError(path)

def load_index(path):
    """Index: list of names; {"units": [...]}; or the combined {"core": {"units": [...]}, "flip": {"pairs": [...]}}.
    Returns unit dicts (flip groups expanded)."""
    d = json.load(open(path))
    if isinstance(d, dict) and "core" in d:
        units = []
        for e in d["core"]["units"]:
            units.append(load_unit(e["unit"], staged=True, block=e.get("block")))
        for g in d.get("flip", {}).get("pairs", []):
            gate = g["gate"]
            units += load_flip(_resolve(g.get("menu_block") or g.get("flip", ""), f"{T2SYNTH}/{gate}/track2_flip.json"))
        return units
    entries = d.get("units", d) if isinstance(d, dict) else d
    units = []
    for e in entries:
        if isinstance(e, str): units.append(load_unit(e, staged=True)); continue
        f = e.get("flip") or (e.get("block") if str(e.get("block", "")).endswith("track2_flip.json") else None)
        if f:
            fp = next((c for c in (os.path.join(BASE, f), os.path.join(SLICES, f), f) if os.path.exists(c)), None)
            if not fp: raise FileNotFoundError(f)
            units += load_flip(fp)
        else:
            units.append(load_unit(e["unit"], staged=True, block=e.get("block")))
    return units

CW_ON = False  # set from --closed-world in main()

def menu_text(u, delivered):
    lines = []
    for n, it in enumerate(u["items"], 1):
        lines.append(f"  {n}. {it['label']}" + ("  (already delivered)" if n in delivered else ""))
    return MENU_HEAD + "\n".join(lines) + MENU_TAIL + (("\n" + CLOSED_WORLD) if CW_ON else "")

# ---------- one unit ----------
def parse_move(text):
    """Last move in the final answer wins: ("pick", n) | ("label", verdict) | ("undecidable", None)."""
    t = text or ""
    moves = [(m.start(), "pick", int(m.group(1))) for m in PICK.finditer(t)]
    moves += [(m.start(), "label", m.group(1).lower()) for m in COMMIT.finditer(t)]
    moves += [(m.start(), "undecidable", None) for m in UNDEC.finditer(t)]
    if not moves: return (None, None)
    # A reply that asks for an item AND carries a need-more-context label means "I want more": the PICK wins
    # (audit 2026-09-26: Foundation-Sec wrote 'PICK: 6  LABEL: need-more-context' on 15/264 unit-runs).
    labels = {v for _, k, v in moves if k == "label"}
    if labels == {"need-more-context"} and any(k == "pick" for _, k, _ in moves):
        moves = [m for m in moves if m[1] == "pick"]
    pos, kind, val = max(moves, key=lambda x: x[0])
    if kind == "label" and val == "undecidable": return ("undecidable", None)
    return (kind, val)

def ask(u, msgs, a, max_tokens, transcript):
    ptok = prompt_tokens(a.port, a.model, msgs)
    budget = min(max_tokens, a.max_model_len - (ptok or 0) - 64)
    if budget < a.min_gen: return None, "context-limit"
    for attempt in range(3):  # gpt-oss occasionally emits a malformed harmony header -> HTTP 500; a resample fixes it
        try:
            aa = a if attempt == 0 else argparse.Namespace(**{**vars(a), "seed": a.seed + 1000 * attempt})  # same seed = same bad sample
            rj = chat(a.port, a.model, msgs, budget, aa, a.reasoning); break
        except urllib.error.HTTPError as e:
            if e.code != 500 or attempt == 2: raise
    ch = rj["choices"][0]; msg = ch["message"]; ans = final_text(msg)
    # reasoning is kept in the saved transcript for diagnosis only; it is never sent back to the model
    reas = msg.get("reasoning_content") or msg.get("reasoning") or ""
    if not reas and "</think>" in (msg.get("content") or ""): reas = msg["content"].split("</think>")[0]
    transcript.append({"role": "assistant", "content": ans, "finish": ch.get("finish_reason"),
                       "out_tokens": rj.get("usage", {}).get("completion_tokens"),
                       "reasoning": (reas[:1500] + " [...] " + reas[-1500:]) if len(reas) > 3000 else reas})
    return ans, ch.get("finish_reason")

def run_unit(u, a):
    t0 = time.time()
    sys_p = SYS_P + (" " + CLOSED_WORLD if getattr(a, "closed_world", False) else "")
    msgs = [{"role": "system", "content": sys_p}, {"role": "user", "content": u["body"] + USR_SUFFIX}]
    tr = [{"role": "user", "content": "<start body>" + USR_SUFFIX}]
    rec = {"unit": u["unit"], "terminal": u["terminal"], "n_on": u["n_on"], "label_fallback": u["label_fallback"],
           "source": u.get("source"), "group": u.get("group"), "role": u.get("role"),
           "deliver_missing": u["deliver_missing"], "menu": [(i["key"], i["kind"], i["label"]) for i in u["items"]],
           # fingerprint of exactly what this unit showed (start body, labels, deliverables) -> detects later data edits
           "data_md5": hashlib.md5(json.dumps([u["terminal"], u["body"], [(i["kind"], i["label"], i["content"])
                                               for i in u["items"]]]).encode()).hexdigest()}
    try:
        ans, fin = ask(u, msgs, a, a.max_tokens, tr)
        if ans is None: return {**rec, "outcome": "context-limit", "transcript": tr}
        first = parse_label(ans) if fin != "length" else None
        rec["first_label"] = first
        if first in ("vulnerable", "not-vulnerable"):
            return {**rec, "outcome": "early-commit", "final": first, "correct": first == u["terminal"],
                    "picks": [], "rounds": 0, "transcript": tr, "latency": round(time.time() - t0, 1)}
        msgs.append({"role": "assistant", "content": ans})
        delivered, picks, cap, nudged = set(), [], u["n_on"] + 2, False
        on_idx = {n for n, it in enumerate(u["items"], 1) if it["kind"] == "on-target"}
        done = lambda **k: {**rec, "picks": picks, "transcript": tr, "latency": round(time.time() - t0, 1), **k}

        def move(prompt, final_only=False):
            """Send one turn, return (kind, val). One short forced retry if the reply has no parseable move."""
            msgs.append({"role": "user", "content": prompt}); tr.append({"role": "user", "content": prompt})
            ans, fin = ask(u, msgs, a, a.max_tokens, tr)
            if ans is None: return ("context-limit", None)
            kind, val = parse_move(ans) if fin != "length" else (None, None)
            if kind is None:
                fm = msgs + [{"role": "assistant", "content": ans[-2000:]},
                             {"role": "user", "content": FINAL_ALL if final_only else FORCE_T2}]
                fbud = 64 if a.reasoning in ("on", "off") else 2048  # gpt-oss thinks before answering, needs room
                r2 = chat(a.port, a.model, fm, fbud, a, "off" if a.reasoning == "on" else a.reasoning)
                a2 = final_text(r2["choices"][0]["message"]); kind, val = parse_move(a2)
                tr.append({"role": "forced", "content": a2}); ans = a2
            msgs.append({"role": "assistant", "content": ans})
            return (kind, val)

        def forced_final(reason, rnd):
            """All case evidence delivered (or cap hit) without a verdict: ask ONCE for a final decision."""
            kind, val = move(FINAL_ALL if reason == "all-delivered" else FINAL_CAP, final_only=True)
            if kind == "context-limit": return done(outcome="context-limit", rounds=rnd)
            if kind == "label" and val in ("vulnerable", "not-vulnerable"):
                return done(outcome="forced-commit", forced_reason=reason, final=val,
                            correct=val == u["terminal"], rounds=rnd + 1)
            return done(outcome="abstain", abstain_how=f"forced:{kind}:{val}", forced_reason=reason,
                        final=None, correct=False, rounds=rnd + 1)

        prompt = menu_text(u, delivered)
        for rnd in range(1, cap + 1):
            kind, val = move(prompt)
            if kind == "context-limit": return done(outcome="context-limit", rounds=rnd)
            if kind == "label" and val in ("vulnerable", "not-vulnerable"):
                return done(outcome="commit", final=val, correct=val == u["terminal"], rounds=rnd)
            if kind == "undecidable":
                return done(outcome="abstain", abstain_how="undecidable", final=None, correct=False, rounds=rnd)
            if kind == "label":  # need-more-context on a menu turn is ambiguous ("I want more")
                if on_idx <= delivered:  # nothing left to deliver -> ask for the final decision
                    return forced_final("all-delivered", rnd)
                if nudged:  # second time -> treat as giving up
                    return done(outcome="abstain", abstain_how="nmc-twice", final=None, correct=False, rounds=rnd)
                nudged = True; picks.append({"n": None, "key": None, "result": "nudge"})
                prompt = NUDGE + menu_text(u, delivered); continue
            if kind == "pick" and 1 <= val <= len(u["items"]):
                it = u["items"][val - 1]
                if val in delivered:
                    picks.append({"n": val, "key": it["key"], "result": "repeat"})
                    prompt = "That item was already delivered.\n\n" + menu_text(u, delivered); continue
                if it["kind"] == "reshow":  # returns the start body; does not end the unit; counts against precision
                    picks.append({"n": val, "key": it["key"], "result": "reshow"})
                    prompt = f"Item {val} ({it['label']}):\n```\n{it['content']}\n```\n\n" + menu_text(u, delivered); continue
                if it["kind"] != "on-target":
                    picks.append({"n": val, "key": it["key"], "result": "off-target"})
                    return done(outcome="off-target", final=None, correct=False, rounds=rnd)
                delivered.add(val)
                picks.append({"n": val, "key": it["key"], "result": "on-target", "src": it["src"]})
                body = it["content"] if it["content"] else "(this item is not available in the stored artifacts)"
                # after the LAST on-target item the model still gets one normal turn to decide on its own
                prompt = f"Item {val} ({it['label']}):\n```\n{body}\n```\n\n" + menu_text(u, delivered)
                continue
            picks.append({"n": val, "key": None, "result": "invalid" if kind == "pick" else "no-move"})
            prompt = "Please make one move.\n\n" + menu_text(u, delivered)
        return forced_final("cap", cap)
    except Exception as e:
        return {**rec, "outcome": "error", "err": str(e)[:300], "transcript": tr}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", required=True); ap.add_argument("--model", required=True)
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--units", nargs="+"); g.add_argument("--index")
    g.add_argument("--flip", nargs="+", help="track2_flip.json files (synthetic flip groups)")
    ap.add_argument("--run-name"); ap.add_argument("--out")
    ap.add_argument("--max-tokens", type=int, default=16384); ap.add_argument("--min-gen", type=int, default=512)
    ap.add_argument("--max-model-len", type=int, default=32768); ap.add_argument("--concurrency", type=int, default=4)
    ap.add_argument("--temperature", type=float, default=0.6); ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--timeout", type=int, default=3600); ap.add_argument("--reasoning-effort", default=None)
    ap.add_argument("--closed-world", action="store_true", help="ablation: tell the model the menu is sufficient")
    ap.add_argument("--no-baits", action="store_true", help="ablation: remove every off-target item from the menu")
    ap.add_argument("--staged", action="store_true", help="read slices/_incoming/_track2/<unit>/track2_block.json when present")
    ap.add_argument("--reasoning", choices=["on", "off", "default"], default="default")
    a = ap.parse_args()
    global CW_ON; CW_ON = a.closed_world
    all_units = load_index(a.index) if a.index else ([u for f in a.flip for u in load_flip(f)] if a.flip else None)
    names = a.units or [u["unit"] for u in all_units]
    name = a.run_name or f"{a.model}__s{a.seed}"
    out = a.out or os.path.join(HERE, "results_t2", f"{name}.jsonl")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    done = set()
    if os.path.exists(out):
        keep = [json.loads(l) for l in open(out) if l.strip()]
        keep = [r for r in keep if r.get("outcome") not in ("error", None)]
        done = {r["unit"] for r in keep}
        with open(out, "w") as f:
            for r in keep: f.write(json.dumps(r) + "\n")
    units = [u for u in all_units if u["unit"] not in done] if all_units else \
            [load_unit(n, a.staged) for n in names if n not in done]
    if a.no_baits:  # best-case ablation: only case items (+ reshow) stay; menu order of the rest is unchanged
        for u in units: u["items"] = [i for i in u["items"] if i["kind"] != "off-target"]
    print(f"[{name}] {len(names)} units, {len(done)} done, {len(units)} to run", flush=True)
    bad = 0; lock = threading.Lock()
    with open(out, "a") as f, ThreadPoolExecutor(max_workers=a.concurrency) as ex:
        for fut in as_completed([ex.submit(run_unit, u, a) for u in units]):
            r = fut.result()
            with lock:
                f.write(json.dumps(r) + "\n"); f.flush(); bad += r["outcome"] == "error"
                print(f"  {r['unit'][:45]:45s} first={r.get('first_label')} outcome={r['outcome']} "
                      f"final={r.get('final')} terminal={r['terminal']} picks={[p['result'] for p in r.get('picks', [])]}",
                      flush=True)
    sys.exit(0 if bad == 0 else 3)

if __name__ == "__main__":
    main()
