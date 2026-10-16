# Track-2 leaderboard (mean ± sd over seeds)

| Setup | seeds | units | solved (own) | solved (forced) | abstain | wrong verdict | acc. when committed | precision | early commit | early right | off-target end | coverage | nmc-start then solved | ctx-limit/err |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Qwen3.8-27B-think | 3 | 41 | 0.512 ± 0.042 | 0.000 ± 0.000 | 0.065 ± 0.037 | 0.081 ± 0.051 | 0.877 ± 0.075 | 0.817 ± 0.026 | 0.073 ± 0.000 | 0.889 ± 0.192 | 0.211 ± 0.037 | 0.472 ± 0.012 | 63/114 | 0/0 |
| Qwen3-Coder-30B | 3 | 41 | 0.276 ± 0.028 | 0.000 ± 0.000 | 0.000 ± 0.000 | 0.098 ± 0.024 | 0.757 ± 0.048 | 0.608 ± 0.054 | 0.341 ± 0.000 | 0.286 ± 0.000 | 0.260 ± 0.028 | 0.534 ± 0.031 | 34/81 | 0/0 |
| Qwen2.5-Coder-32B | 3 | 41 | 0.276 ± 0.037 | 0.000 ± 0.000 | 0.041 ± 0.051 | 0.024 ± 0.024 | 0.921 ± 0.084 | 0.673 ± 0.029 | 0.220 ± 0.042 | 0.371 ± 0.007 | 0.390 ± 0.024 | 0.642 ± 0.024 | 34/96 | 6/0 |
| Qwen3.8-27B-nothink | 3 | 41 | 0.260 ± 0.183 | 0.000 ± 0.000 | 0.285 ± 0.254 | 0.057 ± 0.056 | 0.816 ± 0.088 | 0.774 ± 0.060 | 0.114 ± 0.051 | 0.810 ± 0.330 | 0.285 ± 0.099 | 0.542 ± 0.109 | 32/109 | 0/0 |
| gpt-oss-120b | 3 | 41 | 0.195 ± 0.065 | 0.000 ± 0.000 | 0.073 ± 0.042 | 0.057 ± 0.056 | 0.781 ± 0.206 | 0.749 ± 0.040 | 0.512 ± 0.065 | 0.526 ± 0.028 | 0.154 ± 0.014 | 0.521 ± 0.058 | 24/60 | 0/0 |
| gpt-oss-20b | 3 | 41 | 0.179 ± 0.075 | 0.000 ± 0.000 | 0.033 ± 0.037 | 0.049 ± 0.042 | 0.850 ± 0.132 | 0.754 ± 0.127 | 0.618 ± 0.149 | 0.528 ± 0.048 | 0.089 ± 0.037 | 0.436 ± 0.070 | 22/47 | 0/0 |
| Foundation-Sec-8B | 3 | 41 | 0.033 ± 0.014 | 0.000 ± 0.000 | 0.033 ± 0.037 | 0.073 ± 0.024 | 0.500 ± 0.100 | 0.767 ± 0.252 | 0.805 ± 0.042 | 0.607 ± 0.044 | 0.016 ± 0.014 | 0.086 ± 0.057 | 4/23 | 0/0 |
| Llama-4-Scout | 3 | 41 | 0.016 ± 0.028 | 0.000 ± 0.000 | 0.000 ± 0.000 | 0.008 ± 0.014 | 0.667 | 0.393 ± 0.263 | 0.870 ± 0.037 | 0.645 ± 0.005 | 0.106 ± 0.014 | 0.833 | 2/16 | 0/0 |

## Synthetic flip test (original must end not-vulnerable, each mutant vulnerable; same menu)

| Setup | groups | pairs | pair flip | group flip | same label (surface-pattern) |
|---|---|---|---|---|---|
| gpt-oss-120b | 17 | 28 | 0.405 ± 0.055 | 0.333 ± 0.034 | 0.393 ± 0.062 |
| gpt-oss-20b | 17 | 28 | 0.310 ± 0.090 | 0.275 ± 0.034 | 0.548 ± 0.103 |
| Qwen3-Coder-30B | 17 | 28 | 0.298 ± 0.055 | 0.314 ± 0.034 | 0.571 ± 0.094 |
| Qwen3.8-27B-think | 17 | 28 | 0.214 ± 0.094 | 0.275 ± 0.068 | 0.560 ± 0.074 |
| Foundation-Sec-8B | 17 | 28 | 0.214 ± 0.036 | 0.235 ± 0.059 | 0.524 ± 0.074 |
| Llama-4-Scout | 17 | 28 | 0.214 ± 0.000 | 0.294 ± 0.000 | 0.750 ± 0.000 |
| Qwen3.8-27B-nothink | 17 | 28 | 0.155 ± 0.090 | 0.176 ± 0.059 | 0.667 ± 0.021 |
| Qwen2.5-Coder-32B | 17 | 28 | 0.107 ± 0.000 | 0.176 ± 0.000 | 0.726 ± 0.041 |

solved (own) = correct verdict given unprompted after >=1 delivered item; solved (forced) = correct only when asked for a final decision (all evidence delivered, or round cap); abstain = UNDECIDABLE, need-more-context twice, or no verdict when forced (not counted wrong, not solved); wrong verdict = committed to the wrong label; acc. when committed = correct / (own + forced verdicts); precision = on-target picks / picks (reshow and repeats count against); early = verdict at R0 without asking (flagged, never credited); coverage = on-target items used / on-target items, over committed units.
