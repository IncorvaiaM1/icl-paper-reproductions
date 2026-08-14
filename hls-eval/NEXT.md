# NEXT — frozen plan (2026-08-12)

Decisions are closed. Do not reopen them. If something below fails, the
failure mode is already handled — follow the branch, don't re-plan.

## Locked decisions

- **Qwen anchor:** timeboxed. One pass of `debug_request.py`, 30 min max,
  *after* step 1. Not isolated in that window → drop Qwen, keep DeepSeek-V3
  as the sole paper anchor, and the failure becomes a report finding
  (single-provider routing risk), not a blocker.
- **Synthesis:** Tier C. No Vitis. Three metrics (parseable / compilable /
  runnable). Synthesizability = future work, one sentence in the report.
- **Scope:** the 60 verified designs (polybench 28 + machsuite 18 + c2hlsc 14).

## Execution order — evidence first, debugging second

| # | Action | Time | Rule |
|---|--------|------|------|
| 1 | Smoke-test ALL five models: 1 design × 1 sample each | 10 min | Do this before touching Qwen again. It decides whether Qwen even matters. |
| 2 | `debug_request.py` on Qwen — only if step 1 shows others healthy | ≤30 min | Hard stop at 30 min. No new hypotheses, no new scripts. |
| 3 | Freeze the model list from steps 1–2. Write it in `run_manifest.json` | 2 min | Whatever works is the roster. Minimum viable: DeepSeek-V3 + 2 modern models. |
| 4 | Full sweep: roster × 60 designs × n=5, temp 0.7, 3 metrics | 1–2 h | Start it and walk away. Do NOT watch it and fix things mid-run. |
| 5 | `diagnose_parse.py` over results — split infra failures from model failures | 15 min | Retry infra-failed cells once; then accept. |
| 6 | Aggregate pass@k + bootstrap CIs, figures | 45 min | |
| 7 | Emit real `leaderboard.json` in their schema | 20 min | |
| 8 | Report + email to Callie | 1 h | Lead with "what broke" (FINDINGS.md is mostly written). |

## Stop-rules (the anti-circle mechanism)

1. **No new tooling.** The seven scripts that exist are the toolkit. If a
   problem seems to need an eighth script, the answer is to drop the thing
   causing the problem instead.
2. **Once step 4 starts, the harness is frozen.** Bugs found mid-sweep go
   into FINDINGS.md as text, not into code.
3. **A model that fails twice in the sweep gets dropped from the roster**,
   noted in the report. No per-model debugging after step 3.
4. **The deliverable is the report, not the numbers.** If the sweep produces
   partial data, ship the partial data with honest gaps. FINDINGS.md alone
   already justifies the email.

## What "done" looks like

- `results/` has raw sweep output + aggregated pass@k with CIs
- `leaderboard.json` valid against their schema
- Report: ~2 pages — setup, numbers vs paper's, what broke, fixes offered
- Email: 5 sentences + repo link
