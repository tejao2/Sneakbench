#!/usr/bin/env python3
"""
Track-1 leaderboard over repeated runs. Result files are named <setup>__s<seed>.jsonl.
For each setup: score every seed with score_track1.score_model, then report mean and
standard deviation across seeds, plus run-to-run label agreement and output length.

Usage: python3 aggregate_track1.py results/ > LEADERBOARD.md
"""
import sys, os, glob, json, statistics, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from score_track1 import score_model, LABELS, truth_map, apply_truth

def ms(vals):
    vals = [v for v in vals if v is not None]
    if not vals: return "n/a"
    if len(vals) == 1: return f"{vals[0]:.3f}"
    return f"{statistics.mean(vals):.3f} ± {statistics.stdev(vals):.3f}"

TRUTH = truth_map()

def main(d):
    runs = collections.defaultdict(dict)
    for f in sorted(glob.glob(os.path.join(d, "*__s*.jsonl"))):
        setup, seed = os.path.basename(f)[:-6].rsplit("__s", 1)
        rows = {}
        for l in open(f):
            if l.strip():
                r = json.loads(l); rows[r["id"]] = r   # last row per id wins
        runs[setup][seed] = apply_truth(list(rows.values()), TRUTH)
    n_inst = sum(1 for _ in open(os.path.join(os.path.dirname(os.path.abspath(d)), "instances.jsonl")))

    print("# Track-1 leaderboard (mean ± sd over seeds)\n")
    print(f"Instances: {n_inst}. Temperature 0.6, one seed per run. Metrics per scored instance.\n")
    print("| Setup | runs | complete | macro-F1 | accuracy | over-flag | false-clear | hedge | forced | ctx-miss | avg out tokens | agreement |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|")
    table = []; partial = []
    for setup, allseeds in runs.items():
        # only complete runs enter the averages; partial runs are counted but not averaged
        seeds = {s: rows for s, rows in allseeds.items() if len(rows) >= n_inst}
        if not seeds:
            partial.append(f"{setup}: runs in progress " + ", ".join(f"s{s} {len(r)}/{n_inst}" for s, r in allseeds.items()))
            continue
        S = {s: score_model(rows) for s, rows in seeds.items()}
        # agreement: share of instances where all seeds gave the same label
        preds = collections.defaultdict(list)
        for rows in seeds.values():
            for r in rows:
                if r.get("pred") in LABELS: preds[r["id"]].append(r["pred"])
        full = [v for v in preds.values() if len(v) == len(seeds)]
        agree = (sum(len(set(v)) == 1 for v in full) / len(full)) if full and len(seeds) > 1 else None
        outs = [r.get("completion_tokens") for rows in seeds.values() for r in rows if r.get("completion_tokens")]
        forced = sum(1 for rows in seeds.values() for r in rows if r.get("forced"))
        complete = min(len(rows) for rows in seeds.values())
        f1 = [s["macro_f1"] for s in S.values()]
        table.append((statistics.mean(f1) if f1 else 0, setup, f"{len(seeds)}/{len(allseeds)}", complete, S, agree, outs, forced))
    for _, setup, k, complete, S, agree, outs, forced in sorted(table, reverse=True):
        v = list(S.values())
        print(f"| {setup} | {k} | {complete}/{n_inst} | {ms([s['macro_f1'] for s in v])} | {ms([s['accuracy'] for s in v])} "
              f"| {ms([s['over_flag_rate'] for s in v])} | {ms([s['false_clear_rate'] for s in v])} "
              f"| {ms([s['hedge_rate'] for s in v])} | {forced} | {max(s['context_limit_miss'] for s in v)} "
              f"| {round(statistics.mean(outs)) if outs else 'n/a'} | {f'{agree:.2f}' if agree is not None else 'n/a'} |")
    for p_ in partial: print(f"| {p_} | | | | | | | | | | | |")
    print("\nruns = complete runs averaged / runs started.")
    print("\nover-flag = predicted vulnerable when truth is not; false-clear = predicted not-vulnerable when truth "
          "is not; hedge = predicted need-more-context when truth is decided; forced = answers that needed the "
          "one-line label follow-up (cut off or unlabelled); agreement = share of instances with the same label in "
          "every run.\n")
    print("## Accuracy by category (mean over seeds)\n")
    cats = sorted({c for *_, S, _, _, _ in table for s in S.values() for c in s["by_category"]})
    print("| Setup | " + " | ".join(cats) + " |"); print("|---|" + "---|" * len(cats))
    for _, setup, k, complete, S, *_ in sorted(table, reverse=True):
        cells = []
        for c in cats:
            vals = [s["by_category"][c]["acc"] for s in S.values() if c in s["by_category"]]
            cells.append(f"{statistics.mean(vals):.2f}" if vals else "n/a")
        print(f"| {setup} | " + " | ".join(cells) + " |")
    print("\n## Confusion matrix summed over seeds (rows = truth, cols = predicted: vuln / nmc / not-vuln)\n")
    for _, setup, k, complete, S, *_ in sorted(table, reverse=True):
        print(f"**{setup}**  " + " ; ".join(
            f"{g}: " + "/".join(str(sum(s['confusion'][g][p] for s in S.values())) for p in LABELS) for g in LABELS) + "\n")

if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "results")
