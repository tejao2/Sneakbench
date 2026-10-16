#!/usr/bin/env python3
"""
Track-1 runner: send each benchmark function body to a vLLM OpenAI-compatible server,
parse the 3-way LABEL, and record the result.

v2 (2026-09-25) changes vs run_track1.py.orig, all from the load/timing tests:
  - LABEL is read from the FINAL answer only (text after </think>, or 'content' when the server
    returns reasoning separately), and the LAST 'LABEL:' line wins. Draft labels written inside
    the reasoning are never used.
  - Fallback word search matches need-more-context / not-vulnerable before vulnerable.
  - If the answer was cut off (finish_reason=length) or has no label, ONE short follow-up asks for
    the label only. Such rows are marked forced=true.
  - max_tokens per request = min(--max-tokens, window - real prompt tokens - 64), using the server's
    /tokenize endpoint. If less than --min-gen tokens would remain, the row is context-limit-miss.
  - Sampling: --temperature (default 0.6) and --seed per run, so repeated runs are real repeats.
  - Resume keeps only finished rows (ok / context-limit-miss); failed rows are retried.

Usage:
  python3 run_track1.py --port 8013 --model qwen3coder30b --run-name Qwen3-Coder-30B__s1 --seed 1 \
        [--reasoning on|off|default] [--max-tokens 16384] [--max-model-len 32768] [--concurrency 4]
"""
import os, sys, json, time, argparse, urllib.request, urllib.error, threading, re
from concurrent.futures import ThreadPoolExecutor, as_completed

HERE = os.path.dirname(os.path.abspath(__file__))

SYS_P = ("You are auditing one decompiled firmware function for an authentication or authorization "
         "logic weakness. Decide from THIS function body alone.")
USR_SUFFIX = ("\n\nAnswer with exactly one label: vulnerable, need-more-context, or not-vulnerable. "
              "The FIRST line must be exactly 'LABEL: <label>'. Then give one or two lines of reasoning. "
              "vulnerable = the flaw is complete and provable inside this body. "
              "need-more-context = the function is on the security path but the body alone cannot decide "
              "(it delegates, forwards, or depends on a caller/callee/external state). "
              "not-vulnerable = plumbing, or a correct self-contained gate.")
FORCE_Q = "Stop analysing. Reply with only one line: 'LABEL: <vulnerable|need-more-context|not-vulnerable>'."

LABELS = ("vulnerable", "need-more-context", "not-vulnerable")
LABEL_LINE = re.compile(r"LABEL:\s*[<\[*\"'`]*\s*(need-more-context|not-vulnerable|vulnerable)\b", re.I)
WORD = re.compile(r"\b(need-more-context|not-vulnerable|vulnerable)\b", re.I)
DONE_STATUSES = ("ok", "context-limit-miss")

def final_text(msg):
    """The model's final answer, without reasoning. Qwen-style models inline <think>...</think>."""
    c = msg.get("content") or ""
    return c.split("</think>")[-1] if "</think>" in c else c

def parse_label(text):
    if not text: return None
    m = LABEL_LINE.findall(text)
    if m: return m[-1].lower()
    # fallback: last label word in the tail of the final answer (longer labels matched first)
    w = WORD.findall(text[-400:])
    return w[-1].lower() if w else None

def post(port, path, payload, timeout):
    r = urllib.request.Request(f"http://localhost:{port}{path}", data=json.dumps(payload).encode(),
                               headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(r, timeout=timeout) as resp:
        return json.load(resp)

STRIP = os.environ.get("STRIP_COMMENTS") == "1"   # 2026-09-27 re-run: models see decompiled code without comments

def messages_for(body):
    if STRIP:
        from textclean import strip_comments
        body = strip_comments(body)
    return [{"role": "system", "content": SYS_P}, {"role": "user", "content": body + USR_SUFFIX}]

def prompt_tokens(port, model, msgs):
    try:
        return post(port, "/tokenize", {"model": model, "messages": msgs}, 120).get("count")
    except Exception:
        return None

def chat(port, model, msgs, max_tokens, a, reasoning):
    req = {"model": model, "messages": msgs, "max_tokens": max_tokens,
           "temperature": a.temperature, "seed": a.seed}
    if a.reasoning_effort:  # gpt-oss: low | medium | high (template default is medium)
        req["reasoning_effort"] = a.reasoning_effort
    if reasoning in ("on", "off"):
        req["chat_template_kwargs"] = {"enable_thinking": reasoning == "on"}
    return post(port, "/v1/chat/completions", req, a.timeout)

def run_one(inst, a):
    t0 = time.time()
    msgs = messages_for(inst["body"])
    ptok = prompt_tokens(a.port, a.model, msgs)
    est = ptok if ptok is not None else len(inst["body"]) // 3 + 400
    budget = min(a.max_tokens, a.max_model_len - est - 64)
    if budget < a.min_gen:
        return {**base(inst), "pred": None, "status": "context-limit-miss", "prompt_tokens": est,
                "latency": 0.0}
    try:
        rj = chat(a.port, a.model, msgs, budget, a, a.reasoning)
        ch = rj["choices"][0]; msg = ch["message"]; u = rj.get("usage", {})
        fin = ch.get("finish_reason"); ans = final_text(msg)
        pred = parse_label(ans) if fin != "length" else None
        out_tok = u.get("completion_tokens") or 0
        forced = False
        if pred is None:
            forced = True
            prev = (msg.get("content") or "")[-4000:]
            fmsgs = msgs + [{"role": "assistant", "content": prev}, {"role": "user", "content": FORCE_Q}]
            try:
                r2 = chat(a.port, a.model, fmsgs, 64, a, "off" if a.reasoning == "on" else a.reasoning)
                pred = parse_label(final_text(r2["choices"][0]["message"]))
                out_tok += (r2.get("usage", {}).get("completion_tokens") or 0)
            except Exception:
                pass
        return {**base(inst), "pred": pred, "status": "ok", "forced": forced, "finish_reason": fin,
                "prompt_tokens": u.get("prompt_tokens"), "completion_tokens": out_tok, "max_tokens": budget,
                "latency": round(time.time() - t0, 2), "answer": ans.strip()[:600],
                "reasoning_chars": len(msg.get("reasoning_content") or msg.get("reasoning") or "")
                                   + (len(msg.get("content") or "") - len(ans))}
    except urllib.error.HTTPError as e:
        m = e.read().decode()[:300]
        st = "context-limit-miss" if ("maximum context" in m or "max_model_len" in m) else f"http-{e.code}"
        return {**base(inst), "pred": None, "status": st, "err": m, "latency": round(time.time() - t0, 2)}
    except Exception as e:
        return {**base(inst), "pred": None, "status": "error", "err": str(e)[:300],
                "latency": round(time.time() - t0, 2)}

def base(inst):
    return {"id": inst["id"], "category": inst["category"], "unit": inst.get("unit"),
            "gt_track1": inst["gt_track1"], "gt_terminal": inst.get("gt_terminal"),
            "cwe": inst.get("cwe"), "body_bytes": inst.get("body_bytes")}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", required=True)
    ap.add_argument("--model", required=True)
    ap.add_argument("--instances", default=os.path.join(HERE, "instances.jsonl"))
    ap.add_argument("--out", default=None)
    ap.add_argument("--run-name", default=None)
    ap.add_argument("--max-tokens", type=int, default=16384)
    ap.add_argument("--min-gen", type=int, default=512)
    ap.add_argument("--max-model-len", type=int, default=32768)
    ap.add_argument("--concurrency", type=int, default=4)
    ap.add_argument("--temperature", type=float, default=0.6)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--timeout", type=int, default=3600)
    ap.add_argument("--reasoning-effort", choices=["low", "medium", "high"], default=None)
    ap.add_argument("--reasoning", choices=["on", "off", "default"], default="default")
    a = ap.parse_args()

    name = a.run_name or f"{a.model}__s{a.seed}"
    out = a.out or os.path.join(HERE, "results", f"{name}.jsonl")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    insts = [json.loads(l) for l in open(a.instances)]
    keep = {}
    if os.path.exists(out):
        for l in open(out):
            try:
                r = json.loads(l)
                if r.get("status") in DONE_STATUSES: keep[r["id"]] = r
            except Exception:
                pass
        with open(out, "w") as f:  # drop failed rows so they are retried
            for r in keep.values(): f.write(json.dumps(r) + "\n")
    todo = [i for i in insts if i["id"] not in keep]
    print(f"[{name}] {len(insts)} instances, {len(keep)} done, {len(todo)} to run (port {a.port} "
          f"model {a.model} reasoning {a.reasoning} temp {a.temperature} seed {a.seed} "
          f"maxtok {a.max_tokens} window {a.max_model_len})", flush=True)
    lock = threading.Lock(); n = [0]; t_start = time.time(); bad = [0]
    with open(out, "a") as fout, ThreadPoolExecutor(max_workers=a.concurrency) as ex:
        futs = [ex.submit(run_one, i, a) for i in todo]
        for fut in as_completed(futs):
            row = fut.result()
            with lock:
                fout.write(json.dumps(row) + "\n"); fout.flush()
                n[0] += 1; bad[0] += row["status"] not in DONE_STATUSES
                if n[0] % 10 == 0 or n[0] == len(todo):
                    el = time.time() - t_start
                    print(f"  {n[0]}/{len(todo)} {el/60:.1f}min failed={bad[0]} last={row['id'][-40:]} "
                          f"pred={row.get('pred')} status={row['status']}", flush=True)
    print(f"[{name}] finished: {len(todo)-bad[0]} ok, {bad[0]} failed -> {out}", flush=True)
    sys.exit(0 if bad[0] == 0 else 3)

if __name__ == "__main__":
    main()
