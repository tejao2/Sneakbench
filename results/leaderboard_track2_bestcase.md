# Track-2 leaderboard (mean ± sd over seeds)

| Setup | seeds | units | solved (own) | solved (forced) | abstain | wrong verdict | acc. when committed | precision | early commit | early right | off-target end | coverage | nmc-start then solved | ctx-limit/err |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Qwen3.8-27B-think | 3 | 41 | 0.569 ± 0.028 | 0.000 ± 0.000 | 0.016 ± 0.028 | 0.114 ± 0.014 | 0.856 ± 0.014 | 1.000 ± 0.000 | 0.187 ± 0.014 | 0.655 ± 0.052 | 0.000 ± 0.000 | 0.518 ± 0.021 | 67/91 | 1/0 |
| Qwen3.8-27B-nothink | 3 | 41 | 0.528 ± 0.061 | 0.000 ± 0.000 | 0.114 ± 0.123 | 0.187 ± 0.092 | 0.748 ± 0.071 | 1.000 ± 0.000 | 0.171 ± 0.049 | 0.502 ± 0.146 | 0.000 ± 0.000 | 0.640 ± 0.063 | 65/102 | 0/0 |
| Qwen3-Coder-30B | 3 | 41 | 0.423 ± 0.051 | 0.000 ± 0.000 | 0.008 ± 0.014 | 0.154 ± 0.056 | 0.738 ± 0.085 | 0.953 ± 0.058 | 0.407 ± 0.028 | 0.343 ± 0.056 | 0.000 ± 0.000 | 0.649 ± 0.031 | 52/73 | 0/0 |
| Qwen2.5-Coder-32B | 3 | 41 | 0.382 ± 0.051 | 0.000 ± 0.000 | 0.122 ± 0.098 | 0.081 ± 0.014 | 0.825 ± 0.008 | 0.977 ± 0.040 | 0.341 ± 0.042 | 0.381 ± 0.006 | 0.000 ± 0.000 | 0.802 ± 0.066 | 47/81 | 9/0 |
| gpt-oss-120b | 3 | 41 | 0.228 ± 0.070 | 0.000 ± 0.000 | 0.008 ± 0.014 | 0.065 ± 0.014 | 0.783 ± 0.031 | 1.000 ± 0.000 | 0.683 ± 0.088 | 0.524 ± 0.040 | 0.000 ± 0.000 | 0.528 ± 0.047 | 28/39 | 0/0 |
| gpt-oss-20b | 3 | 41 | 0.179 ± 0.028 | 0.008 ± 0.014 | 0.008 ± 0.014 | 0.065 ± 0.037 | 0.814 ± 0.098 | 1.000 ± 0.000 | 0.650 ± 0.014 | 0.512 ± 0.047 | 0.000 ± 0.000 | 0.375 ± 0.079 | 22/43 | 0/0 |
| Foundation-Sec-8B | 3 | 41 | 0.041 ± 0.037 | 0.016 ± 0.014 | 0.024 ± 0.024 | 0.073 ± 0.073 | 0.568 ± 0.388 | 0.919 ± 0.073 | 0.813 ± 0.070 | 0.571 ± 0.029 | 0.000 ± 0.000 | 0.297 ± 0.189 | 5/23 | 0/0 |
| Llama-4-Scout | 3 | 41 | 0.000 ± 0.000 | 0.000 ± 0.000 | 0.000 ± 0.000 | 0.000 ± 0.000 | n/a | n/a | 1.000 ± 0.000 | 0.593 ± 0.014 | 0.000 ± 0.000 | n/a | 0/0 | 0/0 |

## Synthetic flip test (original must end not-vulnerable, each mutant vulnerable; same menu)

| Setup | groups | pairs | pair flip | group flip | same label (surface-pattern) |
|---|---|---|---|---|---|
| Qwen3.8-27B-think | 17 | 28 | 0.524 ± 0.144 | 0.569 ± 0.090 | 0.417 ± 0.074 |
| gpt-oss-20b | 17 | 28 | 0.440 ± 0.149 | 0.392 ± 0.170 | 0.500 ± 0.156 |
| Qwen3-Coder-30B | 17 | 28 | 0.357 ± 0.036 | 0.314 ± 0.034 | 0.607 ± 0.071 |
| gpt-oss-120b | 17 | 28 | 0.357 ± 0.036 | 0.294 ± 0.000 | 0.488 ± 0.055 |
| Qwen3.8-27B-nothink | 17 | 28 | 0.274 ± 0.055 | 0.275 ± 0.034 | 0.560 ± 0.135 |
| Foundation-Sec-8B | 17 | 28 | 0.226 ± 0.055 | 0.294 ± 0.118 | 0.560 ± 0.149 |
| Llama-4-Scout | 17 | 28 | 0.202 ± 0.021 | 0.294 ± 0.000 | 0.786 ± 0.000 |
| Qwen2.5-Coder-32B | 17 | 28 | 0.202 ± 0.041 | 0.314 ± 0.034 | 0.714 ± 0.000 |

solved (own) = correct verdict given unprompted after >=1 delivered item; solved (forced) = correct only when asked for a final decision (all evidence delivered, or round cap); abstain = UNDECIDABLE, need-more-context twice, or no verdict when forced (not counted wrong, not solved); wrong verdict = committed to the wrong label; acc. when committed = correct / (own + forced verdicts); precision = on-target picks / picks (reshow and repeats count against); early = verdict at R0 without asking (flagged, never credited); coverage = on-target items used / on-target items, over committed units.
