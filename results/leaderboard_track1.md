# Track-1 leaderboard (mean ± sd over seeds)

Instances: 175. Temperature 0.6, one seed per run. Metrics per scored instance.

| Setup | runs | complete | macro-F1 | accuracy | over-flag | false-clear | hedge | forced | ctx-miss | avg out tokens | agreement |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Qwen3.8-27B-think | 3/3 | 175/175 | 0.504 ± 0.021 | 0.592 ± 0.003 | 0.064 ± 0.014 | 0.003 ± 0.005 | 0.705 ± 0.030 | 14 | 0 | 4283 | 0.86 |
| gpt-oss-20b | 3/3 | 175/175 | 0.443 ± 0.002 | 0.451 ± 0.006 | 0.388 ± 0.028 | 0.124 ± 0.030 | 0.299 ± 0.017 | 0 | 0 | 1604 | 0.46 |
| Qwen3.8-27B-nothink | 3/3 | 175/175 | 0.439 ± 0.043 | 0.560 ± 0.011 | 0.081 ± 0.059 | 0.003 ± 0.005 | 0.742 ± 0.095 | 3 | 0 | 123 | 0.82 |
| Qwen3-Coder-30B | 3/3 | 175/175 | 0.404 ± 0.007 | 0.442 ± 0.009 | 0.362 ± 0.012 | 0.017 ± 0.009 | 0.508 ± 0.017 | 0 | 0 | 136 | 0.89 |
| gpt-oss-120b | 3/3 | 175/175 | 0.358 ± 0.017 | 0.375 ± 0.022 | 0.582 ± 0.050 | 0.028 ± 0.005 | 0.273 ± 0.030 | 0 | 0 | 662 | 0.58 |
| Foundation-Sec-8B | 3/3 | 175/175 | 0.317 ± 0.060 | 0.354 ± 0.073 | 0.463 ± 0.069 | 0.103 ± 0.038 | 0.401 ± 0.013 | 3 | 0 | 763 | 0.27 |
| Qwen2.5-Coder-32B | 3/3 | 175/175 | 0.305 ± 0.020 | 0.507 ± 0.006 | 0.060 ± 0.004 | 0.000 ± 0.000 | 0.885 ± 0.020 | 0 | 2 | 79 | 0.98 |
| Llama-4-Scout | 3/3 | 175/175 | 0.232 ± 0.020 | 0.288 ± 0.027 | 0.709 ± 0.056 | 0.000 ± 0.000 | 0.280 ± 0.036 | 0 | 0 | 145 | 0.90 |

runs = complete runs averaged / runs started.

over-flag = predicted vulnerable when truth is not; false-clear = predicted not-vulnerable when truth is not; hedge = predicted need-more-context when truth is decided; forced = answers that needed the one-line label follow-up (cut off or unlabelled); agreement = share of instances with the same label in every run.

## Accuracy by category (mean over seeds)

| Setup | cve | hardneg | injection-supp | qnap-inj | synthetic-mutant | synthetic-original |
|---|---|---|---|---|---|---|
| Qwen3.8-27B-think | 0.63 | 0.50 | 0.47 | 0.00 | 0.49 | 0.86 |
| gpt-oss-20b | 0.45 | 0.23 | 0.33 | 1.00 | 0.66 | 0.37 |
| Qwen3.8-27B-nothink | 0.63 | 0.38 | 0.46 | 0.33 | 0.30 | 0.92 |
| Qwen3-Coder-30B | 0.39 | 0.40 | 0.35 | 0.50 | 0.61 | 0.59 |
| gpt-oss-120b | 0.30 | 0.23 | 0.10 | 0.83 | 0.86 | 0.33 |
| Foundation-Sec-8B | 0.33 | 0.23 | 0.25 | 0.67 | 0.55 | 0.36 |
| Qwen2.5-Coder-32B | 0.58 | 0.38 | 0.50 | 0.00 | 0.10 | 0.98 |
| Llama-4-Scout | 0.17 | 0.00 | 0.14 | 1.00 | 0.86 | 0.27 |

## Confusion matrix summed over seeds (rows = truth, cols = predicted: vuln / nmc / not-vuln)

**Qwen3.8-27B-think**  vulnerable: 43/59/0 ; need-more-context: 23/237/1 ; not-vulnerable: 4/127/31

**gpt-oss-20b**  vulnerable: 72/19/11 ; need-more-context: 115/112/34 ; not-vulnerable: 49/60/53

**Qwen3.8-27B-nothink**  vulnerable: 36/65/1 ; need-more-context: 24/237/0 ; not-vulnerable: 10/131/21

**Qwen3-Coder-30B**  vulnerable: 65/37/0 ; need-more-context: 116/139/6 ; not-vulnerable: 37/97/28

**gpt-oss-120b**  vulnerable: 89/11/2 ; need-more-context: 173/80/8 ; not-vulnerable: 73/61/28

**Foundation-Sec-8B**  vulnerable: 59/34/9 ; need-more-context: 123/108/28 ; not-vulnerable: 72/72/18

**Qwen2.5-Coder-32B**  vulnerable: 18/84/0 ; need-more-context: 13/245/0 ; not-vulnerable: 12/147/0

**Llama-4-Scout**  vulnerable: 90/12/0 ; need-more-context: 203/58/0 ; not-vulnerable: 97/62/3

