#!/usr/bin/env python3
"""
Explain *why* parseability failed, per evaluation.

`can_parse_output` in HLS-Eval is stricter than it sounds. From `eval.py`:

    generated_code = extract_code_xml_from_llm_output(r_text)
    assert len(generated_code) == 1
    assert list(generated_code.keys())[0].endswith(".cpp")

So a response fails "parseable" for any of several distinct reasons, which the
harness collapses into one boolean:

  no_tags        model ignored the XML format (markdown fences, bare code, prose)
  odd_tags       unbalanced open/close - usually a truncated response
  multi_file     valid XML but >1 file returned (e.g. helpfully included the .h)
  not_cpp        single file, but not named *.cpp
  ok             passed

Distinguishing these matters: `no_tags` is an instruction-following failure,
`odd_tags` is likely a token-limit artifact of *your* run rather than a property
of the model, and `multi_file` is arguably the model being helpful. Reporting
them as one number hides all of that.

    python scripts/diagnose_parse.py
    python scripts/diagnose_parse.py --show nussinov      # dump a raw response
"""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent

RE_OPEN = re.compile(r"<OUTPUT_CODE name=\".+\">")
RE_CLOSE = re.compile(r"</OUTPUT_CODE(\s+name=\"(?:\S+)\")?>")
RE_FENCE = re.compile(r"```")


def api_facts(eval_dir: Path) -> dict:
    """Pull finish_reason / token usage out of the saved API response.

    This is the ground truth for 'was the response truncated'. `finish_reason`
    of "length" means the provider hit a token cap; combined with
    completion_tokens it also reveals *which* cap actually applied, which is
    how you tell "my max_tokens was ignored" from "my max_tokens was too low".
    """
    facts: dict = {}
    fp = eval_dir / "single_eval_data.json"
    if not fp.is_file():
        return facts
    try:
        data = json.loads(fp.read_text(errors="replace"))
    except json.JSONDecodeError:
        return facts

    rj = data.get("response_json") or {}
    if isinstance(rj, dict):
        choices = rj.get("choices") or []
        if choices and isinstance(choices[0], dict):
            facts["finish_reason"] = choices[0].get("finish_reason")
        usage = rj.get("usage") or {}
        if isinstance(usage, dict):
            facts["prompt_tokens"] = usage.get("prompt_tokens")
            facts["completion_tokens"] = usage.get("completion_tokens")
        facts["served_model"] = rj.get("model")
        facts["provider"] = rj.get("provider")
    facts["can_parse_output"] = data.get("can_parse_output")
    return facts


def classify(raw: str) -> tuple[str, dict]:
    opens = RE_OPEN.findall(raw)
    closes = RE_CLOSE.findall(raw)
    info = {
        "n_open": len(opens),
        "n_close": len(closes),
        "n_fences": len(RE_FENCE.findall(raw)),
        "chars": len(raw),
        "names": re.findall(r"<OUTPUT_CODE name=\"(.+?)\">", raw),
    }

    total = len(opens) + len(closes)
    if total == 0:
        return "no_tags", info
    if total % 2 != 0:
        return "odd_tags", info
    names = info["names"]
    if len(names) > 1:
        return "multi_file", info
    if names and not names[0].endswith(".cpp"):
        return "not_cpp", info
    return "ok", info


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--raw", type=Path, default=PROJECT_DIR / "results" / "raw")
    ap.add_argument("--show", default=None,
                    help="substring of an eval id; dumps its raw response")
    ap.add_argument("--head", type=int, default=60, help="lines to show with --show")
    args = ap.parse_args()

    if not args.raw.is_dir():
        raise SystemExit(f"no results at {args.raw}")

    outputs = sorted(args.raw.rglob("raw_llm_output.txt"))
    if not outputs:
        raise SystemExit(f"no raw_llm_output.txt under {args.raw}")

    # ------------------------------------------------------------ --show
    if args.show:
        hits = [p for p in outputs if args.show in str(p)]
        if not hits:
            raise SystemExit(f"no eval matching {args.show!r}")
        p = hits[0]
        raw = p.read_text(errors="replace")
        verdict, info = classify(raw)
        facts = api_facts(p.parent)
        print(f"file    : {p}")
        print(f"verdict : {verdict}")
        print(f"detail  : {info}")
        print(f"api     : {facts or '(no response_json saved)'}")
        if facts.get("finish_reason") == "length":
            ct = facts.get("completion_tokens")
            print(
                f"\n  >> finish_reason='length': the provider truncated at "
                f"{ct} completion tokens.\n"
                f"     If that is far below your --max-tokens, the option is "
                f"not reaching the API."
            )
        elif facts.get("finish_reason"):
            print(
                f"\n  >> finish_reason={facts['finish_reason']!r}: NOT a length "
                f"cap. The model stopped on its own, so raising --max-tokens "
                f"will not help."
            )
        print()
        print("-" * 78)
        lines = raw.splitlines()
        for line in lines[: args.head]:
            print(line)
        if len(lines) > args.head:
            print(f"... [{len(lines) - args.head} more lines]")
        print("-" * 78)
        print("\nLAST 15 LINES (truncation shows up here):")
        for line in lines[-15:]:
            print(line)
        return 0

    # ------------------------------------------------------------ summary
    verdicts: Counter = Counter()
    by_model: dict[str, Counter] = defaultdict(Counter)
    rows = []

    finish_reasons: Counter = Counter()
    for p in outputs:
        raw = p.read_text(errors="replace")
        verdict, info = classify(raw)
        facts = api_facts(p.parent)
        verdicts[verdict] += 1
        finish_reasons[facts.get("finish_reason") or "unknown"] += 1

        eval_id = p.parent.parent.name if p.parent.name.startswith("sample") else p.parent.name
        model = eval_id.split("__", 1)[1] if "__" in eval_id else "?"
        by_model[model][verdict] += 1
        rows.append((eval_id, verdict, info, facts))

    n = len(rows)
    print(f"{n} evaluations under {args.raw}\n")
    print("verdict breakdown")
    print("-" * 46)
    for v, c in verdicts.most_common():
        print(f"  {v:<12} {c:>5}  ({100 * c / n:5.1f}%)")

    print("\nfinish_reason (from the provider's own response)")
    print("-" * 46)
    for fr, c in finish_reasons.most_common():
        print(f"  {fr:<12} {c:>5}  ({100 * c / n:5.1f}%)")
    tok = [r[3].get("completion_tokens") for r in rows if r[3].get("completion_tokens")]
    if tok:
        print(f"\n  completion tokens: min {min(tok)}  max {max(tok)}  "
              f"mean {sum(tok) / len(tok):.0f}")

    if len(by_model) > 1:
        print("\nby model")
        print("-" * 78)
        cols = ["ok", "no_tags", "odd_tags", "multi_file", "not_cpp"]
        print(f"  {'model':<44} " + " ".join(f"{c:>9}" for c in cols))
        for m, ctr in sorted(by_model.items()):
            print(f"  {m:<44} " + " ".join(f"{ctr.get(c, 0):>9}" for c in cols))

    bad = [r for r in rows if r[1] != "ok"]
    if bad:
        print(f"\nfailures ({len(bad)}), first 25")
        print("-" * 78)
        for eval_id, verdict, info, facts in bad[:25]:
            fr = facts.get("finish_reason") or "?"
            ct = facts.get("completion_tokens")
            print(
                f"  {eval_id[:42]:<42} {verdict:<11} "
                f"finish={fr:<8} out_tok={str(ct):<6} chars={info['chars']}"
            )
        print(f"\n  inspect one:  python scripts/diagnose_parse.py --show <eval-id>")

    if verdicts.get("odd_tags"):
        print(
            "\nNOTE: 'odd_tags' usually means the response was cut off mid-code.\n"
            "That is a property of the max-token setting in this run, not of the\n"
            "model's HLS ability. Check it before reporting these as failures."
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
