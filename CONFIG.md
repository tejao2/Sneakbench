# Evaluation configuration

## Models and setups
Seven open-weight models in eight setups. Qwen3.8-27B is the only model run twice (thinking
on and off); the rest are distinct models.

| Setup | Type | Thinking mode |
|---|---|---|
| Qwen3.8-27B (thinking / no thinking) | general | toggleable (both run) |
| gpt-oss-20b | general | always on (reasoning model) |
| gpt-oss-120b | general | always on (reasoning model) |
| Qwen3-Coder-30B | code | none |
| Qwen2.5-Coder-32B | code | none |
| Llama-4-Scout | general | none |
| Foundation-Sec-8B | security-tuned | none |

## Run settings
- 3 seeds per setup, temperature 0.6, mean reported.
- Task 1: 24 runs over 175 instances. Task 2: 24 runs over 86 units and pair members
  (41 scored units + 45 flip-pair members).
- Metrics: Task 1 macro-F1, over-flag, hedge; Task 2 outcome shares and pair-flip rate.
  95% intervals are bootstrap over cases; setup comparisons use a paired bootstrap.

## Hardware and serving
- 4x NVIDIA RTX 4500 Ada GPUs (24 GB each), models served locally through an
  OpenAI-compatible endpoint (vLLM).
- Set the model name and port, then point the harness at that endpoint (see README
  quickstart). Per-setup max-token and context-window settings are in the run scripts.

## Reproducing the tables and figures
```bash
python harness/score_track1.py <track1 run jsonls>
python harness/aggregate_track1.py <track1 run dir>      # per-setup Task 1 table
python harness/score_track2.py <track2 run dir> --exclude data/excluded_from_scoring.txt
python figures/plot_paper.py                              # regenerates the figures
```
