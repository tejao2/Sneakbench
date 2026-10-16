# Paper figures (single column) from the v3 leaderboards / STATS_v3.md.
import sys, matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, numpy as np
sys.path.insert(0, ".")
OUT = "./"
plt.rcParams.update({"font.size": 7.5, "font.family": "serif", "pdf.fonttype": 42})
M = ["Qwen3.8-27B-think", "Qwen3.8-27B-nothink", "gpt-oss-20b", "Qwen3-Coder-30B", "gpt-oss-120b", "Foundation-Sec-8B", "Qwen2.5-Coder-32B", "Llama-4-Scout"]
f1 = {"Qwen3.8-27B-think": (0.504,0.423,0.580), "Qwen3.8-27B-nothink": (0.443,0.369,0.515), "gpt-oss-20b": (0.443,0.381,0.502),
      "Qwen3-Coder-30B": (0.404,0.331,0.473), "gpt-oss-120b": (0.358,0.295,0.418), "Foundation-Sec-8B": (0.318,0.272,0.362),
      "Qwen2.5-Coder-32B": (0.305,0.249,0.363), "Llama-4-Scout": (0.232,0.182,0.283)}
conf = {"Qwen3.8-27B-think": [[43,59,0],[23,237,1],[4,127,31]], "gpt-oss-20b": [[72,19,11],[115,112,34],[49,60,53]],
 "Qwen3.8-27B-nothink": [[36,65,1],[24,237,0],[10,131,21]], "Qwen3-Coder-30B": [[65,37,0],[116,139,6],[37,97,28]],
 "gpt-oss-120b": [[89,11,2],[173,80,8],[73,61,28]], "Foundation-Sec-8B": [[59,34,9],[123,108,28],[72,72,18]],
 "Qwen2.5-Coder-32B": [[18,84,0],[13,245,0],[12,147,0]], "Llama-4-Scout": [[90,12,0],[203,58,0],[97,62,3]]}
fig, a = plt.subplots(figsize=(3.45, 2.25)); y = np.arange(len(M))[::-1]
P = np.array([np.sum(conf[m], axis=0) for m in M], float); P /= P.sum(1, keepdims=True); left = np.zeros(len(M))
for j, (lab, c) in enumerate([("vulnerable", "#c44e52"), ("need more context", "#c8c8c8"), ("not vulnerable", "#55a868")]):
    a.barh(y, P[:, j], left=left, color=c, label=lab, height=0.72); left += P[:, j]
for x in (34/175, 121/175): a.axvline(x, ls="--", c="k", lw=0.8)
for yi, m in zip(y, M): a.text(1.02, yi, f"{f1[m][0]:.2f} [{f1[m][1]:.2f}, {f1[m][2]:.2f}]", va="center", fontsize=6.5)
a.text(1.02, len(M)-0.35, "macro-F1 [95% CI]", fontsize=6.5, fontweight="bold")
a.set_yticks(y); a.set_yticklabels(M); a.set_xlim(0, 1); a.set_xlabel("share of answers")
a.legend(loc="upper center", bbox_to_anchor=(0.45, -0.2), ncol=3, frameon=False, fontsize=6.5, handlelength=1)
for s in ("top", "right"): a.spines[s].set_visible(False)
fig.savefig(OUT + "fig_t1_mix.pdf", bbox_inches="tight"); plt.close(fig)

t2 = {"Qwen3.8-27B-think": [0.512,0.081,0.065,0.211,0.073], "Qwen2.5-Coder-32B": [0.276,0.024,0.041,0.390,0.220],
 "Qwen3-Coder-30B": [0.276,0.098,0.000,0.260,0.341], "Qwen3.8-27B-nothink": [0.260,0.057,0.285,0.285,0.114],
 "gpt-oss-120b": [0.195,0.057,0.073,0.154,0.512], "gpt-oss-20b": [0.179,0.049,0.033,0.089,0.618],
 "Foundation-Sec-8B": [0.033,0.073,0.033,0.016,0.805], "Llama-4-Scout": [0.016,0.008,0.000,0.106,0.870]}
T = list(t2); y = np.arange(len(T))[::-1]; V = np.array([t2[m] for m in T]); V = np.c_[V, np.clip(1 - V.sum(1), 0, 1)]
labs = ["solved", "wrong verdict", "abstain", "ended on wrong pick", "answered without asking", "other"]
cols = ["#2e7d32", "#c44e52", "#c8c8c8", "#dd8452", "#8172b3", "#f0f0f0"]
fig, a = plt.subplots(figsize=(3.45, 2.3)); left = np.zeros(len(T))
for j in range(V.shape[1]):
    a.barh(y, V[:, j], left=left, color=cols[j], label=labs[j], height=0.72); left += V[:, j]
for yi, v in zip(y, V[:, 0]):
    if v > 0.1: a.text(v/2, yi, f"{v:.2f}", va="center", ha="center", fontsize=6.5, color="white")
a.set_yticks(y); a.set_yticklabels(T); a.set_xlim(0, 1); a.set_xlabel("share of runs (41 units, 3 seeds)")
a.legend(loc="upper center", bbox_to_anchor=(0.4, -0.2), ncol=3, frameon=False, fontsize=6.5, handlelength=1, columnspacing=1)
for s in ("top", "right"): a.spines[s].set_visible(False)
fig.savefig(OUT + "fig_t2_outcomes.pdf", bbox_inches="tight"); print("ok")
