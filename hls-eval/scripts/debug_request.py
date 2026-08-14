#!/usr/bin/env python3
"""
Bisect a failing HLS-Eval generation request.

A minimal hand-written curl to `qwen/qwen-2.5-coder-32b-instruct` succeeds
(`finish_reason: "stop"`), while the same model called through the harness
returns `finish_reason: "error"` with `prompt_tokens: 0, completion_tokens: 1`
and visibly corrupted output - content dropped from the *middle* of a line, e.g.

    for (int i = ; i < n - d; i++)

So the provider works and something about the harness's request does not. The
candidate differences are: the long structured prompt, the sampling parameters,
and the llm/openai-SDK transport itself.

This script varies one at a time, using raw HTTP so the SDK is out of the
picture unless explicitly tested.

    python scripts/debug_request.py                 # run the full matrix
    python scripts/debug_request.py --design gemm   # different design
"""

from __future__ import annotations

import argparse
import json
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_DIR))


def _find_repo() -> Path:
    cwd = Path.cwd()
    for c in [cwd, *cwd.parents, Path.home() / "randlab" / "hls-eval"]:
        if (c / "hls_eval" / "__init__.py").is_file():
            return c.resolve()
    sys.exit("run from inside the hls-eval checkout")


REPO = _find_repo()
sys.path.insert(0, str(REPO))

from hls_eval.data import BenchmarkCase, find_benchmark_case_dirs  # noqa: E402
from hls_eval.prompts import build_prompt_gen_zero_shot  # noqa: E402

URL = "https://openrouter.ai/api/v1/chat/completions"


def read_key() -> str:
    env = REPO / ".env"
    if env.is_file():
        for line in env.read_text().splitlines():
            if line.startswith("OPENROUTER_API_KEY="):
                return line.split("=", 1)[1].strip()
    sys.exit("no OPENROUTER_API_KEY in .env")


def post(key: str, payload: dict, timeout: float = 180.0) -> dict:
    body = json.dumps(payload).encode()
    req = urllib.request.Request(
        URL, data=body,
        headers={
            "Authorization": f"Bearer {key}",
            "Content-Type": "application/json",
        },
    )
    try:
        with urllib.request.urlopen(req, timeout=timeout) as r:
            return json.load(r)
    except urllib.error.HTTPError as e:
        return {"_http_error": e.code, "_body": e.read().decode()[:400]}
    except Exception as e:  # noqa: BLE001
        return {"_error": f"{type(e).__name__}: {e}"}


def summarize(tag: str, resp: dict) -> bool:
    if "_http_error" in resp:
        print(f"  {tag:<34} HTTP {resp['_http_error']}  {resp['_body'][:120]}")
        return False
    if "_error" in resp:
        print(f"  {tag:<34} {resp['_error'][:120]}")
        return False

    ch = (resp.get("choices") or [{}])[0]
    fr = ch.get("finish_reason")
    nfr = ch.get("native_finish_reason")
    content = (ch.get("message") or {}).get("content") or ""
    usage = resp.get("usage") or {}
    prov = resp.get("provider")
    ok = fr == "stop"
    mark = "OK " if ok else "BAD"
    print(
        f"  {tag:<34} [{mark}] finish={fr}/{nfr} prov={prov} "
        f"in={usage.get('prompt_tokens')} out={usage.get('completion_tokens')} "
        f"chars={len(content)}"
    )
    return ok


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="qwen/qwen-2.5-coder-32b-instruct")
    ap.add_argument("--design", default="nussinov")
    ap.add_argument("--suite", default="polybench")
    ap.add_argument("--sleep", type=float, default=1.0)
    args = ap.parse_args()

    key = read_key()

    # ---------------------------------------------------------- build prompt
    case_dir = REPO / "hls_eval_data" / args.suite / args.design
    if not case_dir.is_dir():
        sys.exit(f"no such design: {case_dir}")
    dirs = [d for d in find_benchmark_case_dirs(REPO / "hls_eval_data")
            if d.name == args.design]
    bc = BenchmarkCase(dirs[0], name=args.design)
    # Same three inputs, in the same order, as eval.py's call site.
    assert len(bc.h_files) == 1
    prompt = build_prompt_gen_zero_shot(
        bc.kernel_description_fp, bc.tb_file, bc.h_files[0]
    )

    print(f"model  : {args.model}")
    print(f"design : {args.design}")
    print(f"prompt : {len(prompt)} chars (~{len(prompt) // 4} tokens)\n")

    msgs = [{"role": "user", "content": prompt}]
    short = [{"role": "user", "content":
              "Write a C function that adds two ints. Code only."}]

    # ---------------------------------------------------------- the matrix
    trials: list[tuple[str, dict]] = [
        ("short prompt, no params", {"model": args.model, "messages": short}),
        ("REAL prompt, no params", {"model": args.model, "messages": msgs}),
        ("REAL + temperature",
         {"model": args.model, "messages": msgs, "temperature": 0.7}),
        ("REAL + temp + max_tokens",
         {"model": args.model, "messages": msgs, "temperature": 0.7,
          "max_tokens": 8192}),
        ("REAL + temp + max_tokens=4096",
         {"model": args.model, "messages": msgs, "temperature": 0.7,
          "max_tokens": 4096}),
        ("REAL + stream=true",
         {"model": args.model, "messages": msgs, "temperature": 0.7,
          "stream": False}),
    ]

    print("raw HTTP (no llm/openai SDK)")
    print("-" * 78)
    results = {}
    for tag, payload in trials:
        results[tag] = summarize(tag, post(key, payload))
        time.sleep(args.sleep)

    # ---------------------------------------------------------- SDK path
    print("\nvia the llm library (what the harness actually uses)")
    print("-" * 78)
    try:
        from hls_eval.llms import build_model_remote_openrouter
        m = build_model_remote_openrouter(args.model, api_key=key)
        r = m.llm.prompt(prompt=prompt, stream=False, temperature=0.7)
        r._force()
        rj = r.json() or {}
        summarize("llm lib, temp only", rj)
    except Exception as e:  # noqa: BLE001
        print(f"  llm lib raised: {type(e).__name__}: {e}")

    # ---------------------------------------------------------- verdict
    print("\n" + "=" * 78)
    if results.get("short prompt, no params") and not results.get("REAL prompt, no params"):
        print("VERDICT: the long structured prompt is what breaks it, not the")
        print("params and not the SDK. Look at prompt size or content.")
    elif results.get("REAL prompt, no params") and not results.get("REAL + temp + max_tokens"):
        print("VERDICT: a sampling parameter breaks it. Bisect temperature vs")
        print("max_tokens from the rows above.")
    elif all(results.values()):
        print("VERDICT: every raw HTTP call succeeded. If the llm-library row")
        print("failed, the transport is at fault, not the request content.")
    else:
        print("VERDICT: mixed - read the rows above; the first BAD row is the")
        print("variable that matters.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
