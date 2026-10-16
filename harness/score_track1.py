#!/usr/bin/env python3
"""
Track-1 scorer. Reads one or more results JSONL files (from run_track1.py) and reports,
per model: 3x3 confusion (gt_track1 x pred), macro P/R/F1, accuracy, coverage, and the
safety metrics over-flag / false-clear / hedge, plus breakdowns by category and CWE.

Usage:
  python3 score_track1.py results/qwen3coder30b.jsonl [more.jsonl ...]
  python3 score_track1.py results/            # scores every *.jsonl in the dir
  python3 score_track1.py results/ --json report.json
"""
import sys, os, json, glob, argparse, collections

LABELS = ["vulnerable", "need-more-context", "not-vulnerable"]

def truth_map():
    """Current ground truth by id from instances.jsonl (next to this script). Scoring always uses it,
    so label corrections apply to result rows written before the correction."""
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "instances.jsonl")
    if not os.path.exists(p): return {}
    return {r["id"]: r for r in (json.loads(l) for l in open(p) if l.strip())}

def apply_truth(rows, truth):
    out = []
    for r in rows:
        t = truth.get(r.get("id"))
        if truth and t is None: continue          # instance no longer in the benchmark
        if t: r = {**r, "gt_track1": t["gt_track1"], "gt_terminal": t.get("gt_terminal")}
        out.append(r)
    return out

def load(paths):
    truth = truth_map()
    files = []
    for p in paths:
        if os.path.isdir(p): files += sorted(glob.glob(os.path.join(p, "*.jsonl")))
        else: files.append(p)
    per = {}
    for f in files:
        model = os.path.splitext(os.path.basename(f))[0]
        per.setdefault(model, [])
        for l in open(f):
            l = l.strip()
            if l: per[model].append(json.loads(l))
    for m in per:  # last row per id wins, then current ground truth
        per[m] = apply_truth(list({r["id"]: r for r in per[m]}.values()), truth)
    return per

def prf(conf):
    """macro precision/recall/F1 over the 3 labels from a confusion dict conf[gt][pred]."""
    ps, rs, fs = [], [], []
    for lb in LABELS:
        tp = conf[lb][lb]
        fp = sum(conf[g][lb] for g in LABELS if g != lb)
        fn = sum(conf[lb][p] for p in LABELS if p != lb)
        p = tp / (tp + fp) if (tp + fp) else 0.0
        r = tp / (tp + fn) if (tp + fn) else 0.0
        f = 2 * p * r / (p + r) if (p + r) else 0.0
        ps.append(p); rs.append(r); fs.append(f)
    return sum(ps) / 3, sum(rs) / 3, sum(fs) / 3, {LABELS[i]: (round(ps[i], 3), round(rs[i], 3), round(fs[i], 3)) for i in range(3)}

def score_model(rows):
    scored = [r for r in rows if r.get("pred") in LABELS and r.get("gt_track1") in LABELS]
    total = len(rows)
    ctx_miss = sum(1 for r in rows if r.get("status") == "context-limit-miss")
    errors = sum(1 for r in rows if r.get("status") not in ("ok", "context-limit-miss"))
    unparsed = sum(1 for r in rows if r.get("status") == "ok" and r.get("pred") not in LABELS)
    conf = {g: {p: 0 for p in LABELS} for g in LABELS}
    for r in scored: conf[r["gt_track1"]][r["pred"]] += 1
    acc = sum(conf[l][l] for l in LABELS) / len(scored) if scored else 0.0
    mp, mr, mf, perlabel = prf(conf)
    # safety metrics (over the scored set)
    over_flag = sum(1 for r in scored if r["pred"] == "vulnerable" and r["gt_track1"] != "vulnerable")
    over_flag_den = sum(1 for r in scored if r["gt_track1"] != "vulnerable")
    false_clear = sum(1 for r in scored if r["pred"] == "not-vulnerable" and r["gt_track1"] != "not-vulnerable")
    false_clear_den = sum(1 for r in scored if r["gt_track1"] != "not-vulnerable")
    hedge = sum(1 for r in scored if r["pred"] == "need-more-context" and r["gt_track1"] != "need-more-context")
    hedge_den = sum(1 for r in scored if r["gt_track1"] != "need-more-context")
    # terminal-level over-flag: predicted vulnerable on a truly not-vulnerable UNIT
    tof = sum(1 for r in scored if r["pred"] == "vulnerable" and r.get("gt_terminal") == "not-vulnerable")
    tof_den = sum(1 for r in scored if r.get("gt_terminal") == "not-vulnerable")
    def rate(n, d): return round(n / d, 3) if d else None
    # breakdown by category (accuracy) and by CWE family
    bycat = {}
    for cat in sorted(set(r["category"] for r in scored)):
        sub = [r for r in scored if r["category"] == cat]
        bycat[cat] = {"n": len(sub), "acc": round(sum(1 for r in sub if r["pred"] == r["gt_track1"]) / len(sub), 3)}
    return {
        "total": total, "scored": len(scored), "context_limit_miss": ctx_miss, "errors": errors, "unparsed": unparsed,
        "accuracy": round(acc, 3), "macro_precision": round(mp, 3), "macro_recall": round(mr, 3), "macro_f1": round(mf, 3),
        "per_label_prf": perlabel,
        "confusion": conf,
        "over_flag_rate": rate(over_flag, over_flag_den), "over_flag": f"{over_flag}/{over_flag_den}",
        "false_clear_rate": rate(false_clear, false_clear_den), "false_clear": f"{false_clear}/{false_clear_den}",
        "hedge_rate": rate(hedge, hedge_den), "hedge": f"{hedge}/{hedge_den}",
        "terminal_overflag_rate": rate(tof, tof_den), "terminal_overflag": f"{tof}/{tof_den}",
        "by_category": bycat,
    }

def print_report(model, s):
    print("=" * 72)
    print(f"MODEL: {model}")
    print(f"  instances {s['total']}  scored {s['scored']}  ctx-limit-miss {s['context_limit_miss']}  "
          f"errors {s['errors']}  unparsed {s['unparsed']}")
    print(f"  accuracy {s['accuracy']}  macro-F1 {s['macro_f1']}  (P {s['macro_precision']} / R {s['macro_recall']})")
    print("  per-label (P,R,F1):", s["per_label_prf"])
    print(f"  OVER-FLAG {s['over_flag']} = {s['over_flag_rate']}   (pred vulnerable when truth is not)")
    print(f"  FALSE-CLEAR {s['false_clear']} = {s['false_clear_rate']}   (pred not-vulnerable when it is not)")
    print(f"  HEDGE {s['hedge']} = {s['hedge_rate']}   (pred need-more-context on a decided truth)")
    print(f"  TERMINAL-OVER-FLAG {s['terminal_overflag']} = {s['terminal_overflag_rate']}   (pred vulnerable on a not-vulnerable UNIT)")
    print("  confusion  rows=gt cols=pred  [vuln, nmc, notvuln]:")
    for g in LABELS:
        print(f"     {g:17s} -> " + "  ".join(f"{s['confusion'][g][p]:3d}" for p in LABELS))
    print("  by category (n, acc):", {k: (v["n"], v["acc"]) for k, v in s["by_category"].items()})

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="+")
    ap.add_argument("--json", default=None, help="write full report as JSON")
    args = ap.parse_args()
    per = load(args.paths)
    report = {}
    for model in sorted(per):
        s = score_model(per[model]); report[model] = s; print_report(model, s)
    if args.json:
        json.dump(report, open(args.json, "w"), indent=1); print(f"\nwrote {args.json}")
    # leaderboard
    print("\n" + "=" * 72 + "\nLEADERBOARD (macro-F1, then low over-flag):")
    lb = sorted(report.items(), key=lambda kv: (-(kv[1]["macro_f1"] or 0), kv[1]["over_flag_rate"] or 1))
    for m, s in lb:
        print(f"  {m:20s} F1 {s['macro_f1']}  acc {s['accuracy']}  overflag {s['over_flag_rate']}  "
              f"false-clear {s['false_clear_rate']}  (scored {s['scored']}, ctx-miss {s['context_limit_miss']})")

if __name__ == "__main__":
    main()
