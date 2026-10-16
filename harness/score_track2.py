#!/usr/bin/env python3
"""
Track-2 scorer. Reads results_t2/<setup>__s<seed>.jsonl (from run_track2.py) and reports, per setup,
mean +- sd over seeds of the measures agreed with benchmark-v2 (session_buffer.md 2026-09-25 17:42):

  solved_own / solved_forced / abstain / wrong / acc_committed   see the legend printed under the table
  precision    on-target picks / all picks (repeat, reshow and off-target picks count against it)
  early        share of units committed at R0 without asking (flagged); early_right = of those, correct
  off_target   share of units ended by an off-target pick
  coverage     mean (on-target items delivered / on-target items in the unit), over committed units
  flip         synthetic pairs only (units named <gate>-M<k> / original): share of pairs where the mutant
               ends vulnerable AND the original ends not-vulnerable (reported when such pairs exist)

Usage: python3 score_track2.py results_t2/ [--md out.md]
"""
import sys, os, glob, json, statistics, collections, argparse

def ms(v):
    v = [x for x in v if x is not None]
    if not v: return "n/a"
    return f"{v[0]:.3f}" if len(v) == 1 else f"{statistics.mean(v):.3f} ± {statistics.stdev(v):.3f}"

def score(rows):
    """Outcomes (run_track2.py, moves agreed 2026-09-25): early-commit | commit (own verdict) | forced-commit (verdict
    only when asked after all evidence / at the cap) | abstain (UNDECIDABLE, nmc twice, or forced non-verdict) |
    off-target (bait pick = fail) | context-limit | error."""
    n = len(rows)
    if not n: return None
    picks = [p for r in rows for p in r.get("picks", [])]
    on = sum(p["result"] == "on-target" for p in picks)
    real = [p for p in picks if p["result"] in ("on-target", "off-target", "repeat", "reshow")]
    has_ctx = lambda r: any(p["result"] == "on-target" for p in r.get("picks", []))
    own = [r for r in rows if r["outcome"] == "commit"]
    forced = [r for r in rows if r["outcome"] == "forced-commit"]
    committed = own + forced
    early = [r for r in rows if r["outcome"] == "early-commit"]
    cov = [sum(p["result"] == "on-target" for p in r.get("picks", [])) / r["n_on"]
           for r in committed if r.get("n_on")]
    nmc0 = [r for r in rows if r.get("first_label") == "need-more-context"]
    solved_own = [r for r in own if r.get("correct") and has_ctx(r)]
    return {"n": n,
            "solved_own": len(solved_own) / n,
            "solved_forced": sum(bool(r.get("correct")) for r in forced) / n,
            "abstain": sum(r["outcome"] == "abstain" for r in rows) / n,
            "wrong": sum(not r.get("correct") for r in committed) / n,
            "acc_committed": (sum(bool(r.get("correct")) for r in committed) / len(committed)) if committed else None,
            "precision": (on / len(real)) if real else None,
            "early": len(early) / n, "early_right": (sum(bool(r.get("correct")) for r in early) / len(early)) if early else None,
            "off_target": sum(r["outcome"] == "off-target" for r in rows) / n,
            "ctx_limit": sum(r["outcome"] == "context-limit" for r in rows),
            "errors": sum(r["outcome"] == "error" for r in rows),
            "coverage": statistics.mean(cov) if cov else None,
            "nmc_start": len(nmc0), "nmc_then_solved": sum(r in solved_own for r in nmc0),
            "label_fallback": sum(bool(r.get("label_fallback")) for r in rows),
            "deliver_missing_units": sum(bool(r.get("deliver_missing")) for r in rows)}

def flip_score(rows):
    """Synthetic flip groups: original (expected not-vulnerable) and mutants (expected vulnerable) share one menu.
    pair_flip = share of (original, mutant) pairs with original -> not-vulnerable AND mutant -> vulnerable.
    group_flip = share of groups where the original and ALL mutants are right. same_label = pairs with identical finals
    (reacting to surface pattern, not to the gate)."""
    g = collections.defaultdict(dict)
    for r in rows:
        if r.get("group"): g[r["group"]].setdefault(r["role"], []).append(r)
    pairs = same = ok = groups_ok = 0
    for grp in g.values():
        o = (grp.get("original") or [None])[0]; ms = grp.get("mutant", [])
        if not o or not ms: continue
        of = o.get("final")
        for m in ms:
            pairs += 1; same += m.get("final") == of
            ok += of == "not-vulnerable" and m.get("final") == "vulnerable"
        groups_ok += of == "not-vulnerable" and all(m.get("final") == "vulnerable" for m in ms)
    n = sum(1 for grp in g.values() if grp.get("original") and grp.get("mutant"))
    return {"groups": n, "pairs": pairs, "pair_flip": ok / pairs if pairs else None,
            "group_flip": groups_ok / n if n else None, "same_label": same / pairs if pairs else None}

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("d"); ap.add_argument("--md")
    ap.add_argument("--exclude", default=None, help="file with unit names to leave out of scoring (run_excluded)")
    a = ap.parse_args()
    excl = set(open(a.exclude).read().split()) if a.exclude else set()
    runs = collections.defaultdict(dict)
    for f in sorted(glob.glob(os.path.join(a.d, "*__s*.jsonl"))):
        setup, seed = os.path.basename(f)[:-6].rsplit("__s", 1)
        rows = {}
        for l in open(f):
            if l.strip():
                r = json.loads(l)
                if r["unit"] not in excl: rows[r["unit"]] = r
        runs[setup][seed] = list(rows.values())
    flips = {st: {sd: [r for r in rs if r.get("group")] for sd, rs in sd_.items()} for st, sd_ in runs.items()}
    runs = {st: {sd: [r for r in rs if not r.get("group")] for sd, rs in sd_.items()} for st, sd_ in runs.items()}
    out = ["# Track-2 leaderboard (mean ± sd over seeds)\n",
           "| Setup | seeds | units | solved (own) | solved (forced) | abstain | wrong verdict | acc. when committed | "
           "precision | early commit | early right | off-target end | coverage | nmc-start then solved | ctx-limit/err |",
           "|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|"]
    table = []
    for setup, seeds in runs.items():
        S = [score(r) for r in seeds.values() if r]
        if not S: continue
        table.append((statistics.mean(s["solved_own"] for s in S), setup, S))
    for _, setup, S in sorted(table, reverse=True):
        g = lambda k: ms([s[k] for s in S])
        out.append(f"| {setup} | {len(S)} | {S[0]['n']} | {g('solved_own')} | {g('solved_forced')} | {g('abstain')} | "
                   f"{g('wrong')} | {g('acc_committed')} | {g('precision')} | {g('early')} | {g('early_right')} | "
                   f"{g('off_target')} | {g('coverage')} | "
                   f"{sum(s['nmc_then_solved'] for s in S)}/{sum(s['nmc_start'] for s in S)} | "
                   f"{sum(s['ctx_limit'] for s in S)}/{sum(s['errors'] for s in S)} |")
    F = {st: [flip_score(r) for r in sd.values() if r] for st, sd in flips.items()}
    if any(F.values()):
        out += ["\n## Synthetic flip test (original must end not-vulnerable, each mutant vulnerable; same menu)\n",
                "| Setup | groups | pairs | pair flip | group flip | same label (surface-pattern) |", "|---|---|---|---|---|---|"]
        for st, fs in sorted(F.items(), key=lambda kv: -statistics.mean([f["pair_flip"] or 0 for f in kv[1]] or [0])):
            if not fs: continue
            out.append(f"| {st} | {fs[0]['groups']} | {fs[0]['pairs']} | {ms([f['pair_flip'] for f in fs])} | "
                       f"{ms([f['group_flip'] for f in fs])} | {ms([f['same_label'] for f in fs])} |")
    warn = [(setup, sum(s["label_fallback"] for s in S), sum(s["deliver_missing_units"] for s in S)) for _, setup, S in table]
    if any(w[1] or w[2] for w in warn):
        out.append("\nDATA WARNINGS (unit-runs): " + "; ".join(f"{s}: fallback labels {x}, missing deliver {y}" for s, x, y in warn))
    out.append("\nsolved (own) = correct verdict given unprompted after >=1 delivered item; solved (forced) = correct "
               "only when asked for a final decision (all evidence delivered, or round cap); abstain = UNDECIDABLE, "
               "need-more-context twice, or no verdict when forced (not counted wrong, not solved); wrong verdict = "
               "committed to the wrong label; acc. when committed = correct / (own + forced verdicts); precision = "
               "on-target picks / picks (reshow and repeats count against); early = verdict at R0 without asking "
               "(flagged, never credited); coverage = on-target items used / on-target items, over committed units.")
    text = "\n".join(out); print(text)
    if a.md: open(a.md, "w").write(text + "\n")

if __name__ == "__main__":
    main()
