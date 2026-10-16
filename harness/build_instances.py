#!/usr/bin/env python3
"""
Track-1 instance builder for the SneakBench firmware auth/authz benchmark.

Track-1 = show ONE decompiled function body in isolation, model gives a 3-way label
(vulnerable | need-more-context | not-vulnerable), scored against the body-only ground truth.

This enumerates every COUNTED body-only slice from the folded canonical set and writes one
JSONL row per instance. Big-context (oversized) units are EXCLUDED here: by our own rule they
carry no Track-1 label and are Track-2 / context-window units only.

Row schema:
  { id, category, unit, body, body_bytes,
    gt_track1,        # body-only correct label (what a perfect body-only reader says)
    gt_terminal,      # the unit's true thread terminal (vulnerable|not-vulnerable|residual)
    cwe, source_rel, va }

Usage:  python3 build_instances.py [--out instances.jsonl]
Prints a summary (counts by category and by gt_track1) so the set can be sanity-checked.
"""
import json, os, glob, argparse, collections, re

BASE = "/path/to/thesis/firmauth/k1k2_localization"
NORM = {"vulnerable": "vulnerable", "vuln": "vulnerable",
        "not-vulnerable": "not-vulnerable", "not_vulnerable": "not-vulnerable", "notvulnerable": "not-vulnerable",
        "need-more-context": "need-more-context", "nmc": "need-more-context", "need_more_context": "need-more-context"}

def norm(lbl):
    if lbl is None: return None
    s = str(lbl).strip().lower()
    # take the first recognizable token
    for k, v in NORM.items():
        if s == k: return v
    m = re.search(r"(vulnerable|need-more-context|not-vulnerable)", s)
    return m.group(1) if m else None

def first_label(sl, order=("blind", "reconciled", "blind_verdict", "reconciled_label")):
    for k in order:
        v = sl.get(k)
        if v and not str(v).lower().startswith("not-run"):
            n = norm(v)
            if n: return n
    return None

def counted(d):
    return d.get("counted", True) is not False

def terminal_label(d):
    t = d.get("terminal")
    if isinstance(t, dict): t = t.get("label")
    return norm(t)

def _va_keys(sl):
    """Address strings to look for in file names, without 0x and without leading zeros."""
    raw = str(sl.get("va") or sl.get("target") or "")
    keys = []
    for part in re.split(r"[/,\s]+", raw):
        h = part.lower().replace("0x", "").lstrip("0")
        if h: keys.append(h)
    return keys

# Ground-truth corrections agreed with benchmark-v2 (session_buffer.md, 2026-09-25 04:45).
# The hardneg registry "blind" field stores a MODEL answer for over-flag traps, not ground truth.
GT_OVERRIDES = {
    "hardneg/zyxel-000114b4-weblogin": "need-more-context",  # strcmp admin/1234 -> bare bool; real gate is PAM in caller
}
CVE_CTX = f"{BASE}/slices/_incoming/cve_ctx"

def slice_body(sl, unit_dir):
    """Body text of a slice: inline 'body', else a decompiled .c file whose name holds the address
    (unit dir first, then ctx/). Leading zeros in the recorded address are ignored."""
    if sl.get("body"): return sl["body"]
    if not unit_dir: return None
    for h in _va_keys(sl):
        for pat in (f"fn_0x{h}*.c", f"*0x{h}*.c", f"ctx/*0x{h}*.c", f"ctx/*{h}*.c"):
            for p in sorted(glob.glob(os.path.join(unit_dir, pat))):
                try: return open(p).read()
                except Exception: pass
    # staged body-only body from benchmark-v2 for units whose slice body was not decompiled in place
    staged = os.path.join(CVE_CTX, os.path.basename(unit_dir.rstrip("/")), "blind_body.c")
    if os.path.exists(staged): return open(staged).read()
    return None

NO_BODY = []
def add(rows, **kw):
    if kw.get("id") in GT_OVERRIDES: kw["gt_track1"] = GT_OVERRIDES[kw["id"]]
    if kw.get("body") is not None and not str(kw["body"]).strip(): kw["body"] = None  # empty file = no body
    if kw.get("body") and kw.get("gt_track1"):
        kw["body_bytes"] = len(kw["body"])
        rows.append(kw)
    else:
        NO_BODY.append((kw.get("id"), "no-body" if not kw.get("body") else "no-label"))

def build():
    rows = []

    # 1) CVE units (slices/cve). counted only. big-context (slices/big-context) is EXCLUDED from Track-1.
    for sj in sorted(glob.glob(f"{BASE}/slices/cve/*/slices.json")):
        d = json.load(open(sj))
        if not counted(d): continue
        unit = os.path.basename(os.path.dirname(sj)); unit_dir = os.path.dirname(sj)
        gt_term = terminal_label(d)
        for sl in d.get("slices", []):
            # respect per-slice counted flags used in the corpus
            if sl.get("counted") is False: continue
            if "not counted" in str(sl.get("reconcile_note", "")).lower():
                continue  # explicitly uncounted slice (e.g. off-path, rule 2b)
            if str(sl.get("blind", "")).lower().startswith("not-run"):
                continue  # never judged body-only (oversized) -> not a Track-1 instance
            # CVE branch: the adjudicated label is ground truth, the blind reader's label only a fallback
            # (fix 2026-09-26 from benchmark-v2: blind was winning where the two disagree; 7 rows)
            lbl = first_label(sl, ("reconciled", "reconciled_label", "blind", "blind_verdict"))
            body = slice_body(sl, unit_dir)
            add(rows, id=f"cve/{unit}/{sl.get('va') or sl.get('target')}", category="cve", unit=unit,
                body=body, gt_track1=lbl, gt_terminal=gt_term, cwe=d.get("cwe"),
                source_rel=d.get("binary"), va=sl.get("va") or sl.get("target"))

    # 2) Synthetic mutants (counted) : gt_track1 = meta blind_verdict (mostly vulnerable). gt_terminal vulnerable.
    reg = json.load(open(f"{BASE}/slices/non-cves/synthetic/synthetic_gates.json"))
    for r in reg:
        mid = r["id"]; d = f"{BASE}/slices/non-cves/synthetic/{mid}"
        mp = os.path.join(d, "mutated_vulnerable.c"); meta = os.path.join(d, "meta.json")
        if not os.path.exists(mp): continue
        m = json.load(open(meta)) if os.path.exists(meta) else {}
        add(rows, id=f"synth-mut/{mid}", category="synthetic-mutant", unit=mid,
            body=open(mp).read(), gt_track1=norm(m.get("blind_verdict") or "vulnerable"),
            gt_terminal="vulnerable", cwe=m.get("cwe") or m.get("mutation_name"),
            source_rel=m.get("source_rel"), va=m.get("source_va"))

    # 3) Synthetic originals (counted not-vulnerable pole) : gt_track1 = patched_original_blind (all nmc by finding).
    op = f"{BASE}/slices/non-cves/synthetic/synthetic_originals.json"
    if os.path.exists(op):
        oj = json.load(open(op))
        originals = oj.get("originals") if isinstance(oj, dict) else oj
        for r in originals:
            oid = r.get("gate") or r.get("id") or r.get("representative_id")
            opath = r.get("original_body")
            if opath and not os.path.isabs(opath): opath = os.path.join(BASE, opath)
            if not opath or not os.path.exists(opath): continue
            meta = os.path.join(os.path.dirname(opath), "meta.json")
            m = json.load(open(meta)) if os.path.exists(meta) else {}
            add(rows, id=f"synth-orig/{oid}", category="synthetic-original", unit=oid,
                body=open(opath).read(), gt_track1=norm(m.get("patched_original_blind")) or "need-more-context",
                gt_terminal="not-vulnerable", cwe=m.get("cwe"), source_rel=r.get("source_rel"), va=r.get("source_va"))

    # 4) Hardneg (counted not-vulnerable) : gt_track1 = recorded body-only blind if present, else not-vulnerable.
    hn = json.load(open(f"{BASE}/slices/non-cves/hardneg/hardneg_registry.json"))
    for r in hn:
        hid = r["id"]; d = f"{BASE}/slices/non-cves/hardneg/{hid}"
        fnp = os.path.join(d, "fn.c")
        if not os.path.exists(fnp):
            alt = glob.glob(os.path.join(d, "*.c"))
            fnp = alt[0] if alt else None
        if not fnp or not os.path.exists(fnp): continue
        gt = norm(r.get("blind_verdict") or r.get("blind"))
        if not gt: continue  # no body-only verdict recorded -> not a counted hardneg (matches recount)
        add(rows, id=f"hardneg/{hid}", category="hardneg", unit=hid, body=open(fnp).read(),
            gt_track1=gt, cwe=r.get("why"), source_rel=r.get("rel"), va=r.get("va"),
            # 2026-09-27 (benchmark-v2 A-flips): a hardneg whose privileged op is reachable without login carries
            # terminal_override / context_terminal_label = vulnerable; otherwise the hardneg terminal is not-vulnerable
            gt_terminal=norm(r.get("terminal_override") or r.get("context_terminal_label")) or "not-vulnerable")

    # 5) Injection-supp : per-slice body-only blind_verdict (slices.json shape).
    for sj in sorted(glob.glob(f"{BASE}/slices/non-cves/injection-supp/*/slices.json")):
        d = json.load(open(sj))
        if not counted(d): continue
        unit = os.path.basename(os.path.dirname(sj)); unit_dir = os.path.dirname(sj)
        # injection-supp keeps its terminal in track2.terminal (top-level "terminal" is null), per benchmark-v2
        t2 = d.get("track2") if isinstance(d.get("track2"), dict) else {}
        gt_term = norm(t2.get("terminal")) or terminal_label(d)
        for sl in d.get("slices", []):
            if sl.get("counted") is False: continue
            lbl = norm(sl.get("blind_verdict") or sl.get("blind") or sl.get("reconciled") or sl.get("proposed_label"))
            body = slice_body(sl, unit_dir) or sl.get("body")
            add(rows, id=f"injection-supp/{unit}/{sl.get('va') or sl.get('target')}", category="injection-supp", unit=unit,
                body=body, gt_track1=lbl, gt_terminal=norm(sl.get("context_terminal_label")) or gt_term, cwe=d.get("cwe"),
                source_rel=d.get("binary"), va=sl.get("va") or sl.get("target"))

    # 6) qnap-inj : fn.c + meta.json shape (like hardneg). Injection backdoors -> vulnerable in-body.
    for d in sorted(glob.glob(f"{BASE}/slices/non-cves/qnap-inj/*/")):
        unit = os.path.basename(d.rstrip("/")); fnp = os.path.join(d, "fn.c")
        if not os.path.exists(fnp): continue
        m = json.load(open(os.path.join(d, "meta.json"))) if os.path.exists(os.path.join(d, "meta.json")) else {}
        gt = norm(m.get("blind_verdict") or m.get("label") or m.get("terminal")) or "vulnerable"
        add(rows, id=f"qnap-inj/{unit}", category="qnap-inj", unit=unit, body=open(fnp).read(),
            gt_track1=gt, gt_terminal=norm(m.get("terminal")) or "vulnerable",
            cwe=m.get("cwe"), source_rel=m.get("source_rel") or m.get("rel"), va=m.get("source_va") or m.get("va"))
    return rows

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.join(os.path.dirname(__file__), "instances.jsonl"))
    args = ap.parse_args()
    rows = build()
    with open(args.out, "w") as f:
        for r in rows: f.write(json.dumps(r) + "\n")
    bycat = collections.Counter(r["category"] for r in rows)
    bygt = collections.Counter(r["gt_track1"] for r in rows)
    missing_gt = [r["id"] for r in rows if not r["gt_track1"]]
    print(f"wrote {len(rows)} Track-1 instances -> {args.out}")
    print("by category:", dict(bycat))
    print("by gt_track1:", dict(bygt))
    if NO_BODY: print(f"NOT INCLUDED ({len(NO_BODY)}):", NO_BODY)
    if missing_gt: print(f"WARNING: {len(missing_gt)} rows missing gt_track1 (dropped by add()):", missing_gt[:8])
    big = len(glob.glob(f"{BASE}/slices/big-context/*/slices.json"))
    print(f"(big-context units excluded from Track-1: {big} dirs)")

if __name__ == "__main__":
    main()
