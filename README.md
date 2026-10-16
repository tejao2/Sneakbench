# SneakBench

A CVE-confirmed benchmark of **authentication and authorization logic bugs in firmware
binaries**, with an LLM evaluation. The flaw is in the access decision itself: a request
reaches a privileged operation without valid credentials or the right privilege level.

Many of these bugs cannot be decided from a single function, so the benchmark has two
tasks:

- **Task 1 (abstention).** The model sees one decompiled function and labels it
  `vulnerable`, `not-vulnerable`, or `need-more-context`. It is scored on knowing when the
  body alone is not enough.
- **Task 2 (investigation).** The model starts from one function of a multi-function
  *unit* and a numbered menu of context items. It asks for items, then gives a verdict. An
  off-target pick ends the run.

## Benchmark at a glance
- **26 confirmed CVEs**, across router, camera, and NAS firmware.
- **Task 1: 175 instances** (34 vulnerable / 87 need-more-context / 54 not-vulnerable),
  made of 92 CVE-derived slices, 46 synthetic, 21 command-injection, and 16 hard negatives.
- **Task 2: 44 units** (3 big-context units held out of the main run -> **41 scored**),
  plus **17 synthetic flip-pair groups** (17 originals + 28 variants = 45 instances).

## Leaderboard (seven open-weight models, eight setups, 3 seeds, mean)

| Setup | Task 1 macro-F1 | Task 2 solved | Flip-rate | Task 2 solved (best case) |
|---|---|---|---|---|
| Qwen3.8-27B (thinking) | **0.50** | **0.51** | 0.29 | **0.57** |
| Qwen3-Coder-30B | 0.40 | 0.28 | 0.38 | 0.42 |
| Qwen2.5-Coder-32B | 0.31 | 0.28 | 0.18 | 0.38 |
| Qwen3.8-27B (no thinking) | 0.44 | 0.26 | 0.21 | 0.53 |
| gpt-oss-120b | 0.36 | 0.20 | **0.43** | 0.23 |
| gpt-oss-20b | 0.44 | 0.18 | 0.34 | 0.18 |
| Foundation-Sec-8B | 0.32 | 0.03 | 0.25 | 0.04 |
| Llama-4-Scout | 0.23 | 0.02 | 0.32 | 0.00 |

*Task 1 macro-F1* over the three labels; *Task 2 solved* = correct verdict after asking for
at least one item; *Flip-rate* = share of synthetic pairs the model splits correctly;
*best case* = Task 2 with only the right items in the menu. Full numbers with 95% intervals
and paired tests are in [`results/STATS.md`](results/STATS.md).

## Repository layout
```
data/
  track1_instances.jsonl      Task 1: 175 body-only instances (body, label, category, CWE)
  track2_units/               Task 2: units (per-unit block + ctx/ decompiled slices)
  track2_flip/                synthetic flip pairs (original + mutated variants)
  track2_index.json           core + flip + totals index
  excluded_from_scoring.txt   units held out of the main run (big-context + one pole)
  cve_list.csv                the CVEs behind the benchmark (device, CWE, slice count)
harness/                      run_track1/2.py, score_track1/2.py, aggregate_track1.py, textclean.py
prompts/PROMPTS.md            the exact Task 1 / Task 2 / best-case prompts
results/                      STATS.md + per-setup leaderboards (Task 1, Task 2, best case)
figures/plot_paper.py         regenerates the result figures
DATA_CARD.md                  field-by-field schema and label definitions
CONFIG.md                     models, setups, seeds, hardware, serving
```

## Quickstart
```bash
pip install -r harness/requirements.txt
# serve a model with an OpenAI-compatible endpoint (e.g. vLLM), then:
python harness/run_track1.py --model <name> --port <p> --out runs/t1.jsonl
python harness/score_track1.py runs/t1.jsonl
python harness/run_track2.py --index data/track2_index.json --model <name> --port <p> --out runs/t2.jsonl
python harness/score_track2.py runs/
```
See [`CONFIG.md`](CONFIG.md) for the exact models, seeds, and serving setup.

## Notes
- The `ctx/*.c` slices and unit blocks keep analyst annotations for transparency. The
  harness **strips all comments** before showing a function to a model (`harness/textclean.py`),
  and rejects any item that still contains answer-revealing text. If you evaluate a model
  by hand, strip comments first.
- No vendor firmware binaries are redistributed. `data/cve_list.csv` identifies the exact
  builds; the benchmark ships only the decompiled slices a model needs to read.

## License
See [`LICENSE.md`](LICENSE.md).
