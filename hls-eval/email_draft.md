Subject: HLS-Eval + analog benchmarks — reproductions, and a first-project idea

Hi Callie,

I read a slice of your group's papers and reproduced what I could run from outside the lab, without Vitis or an FPAA board. The write-up is attached (also at [REPO LINK]).

HLS-Eval (ICLAD 2025): zero-shot generation on 57 designs, parse/compile/run, n=5. Your DeepSeek-V3 anchor is 46% run@1 here vs 63% in the paper, but 63% vs 66% at run@5 — the pass@1 gap looks like the harness's max_tokens=8192 truncating completions (21% of calls ended with finish_reason=length), not a capability miss. Under that same cap, Qwen3-Coder reaches 67% run@1. I had to drop the Qwen2.5-Coder-32B anchor after a single-provider (Cloudflare) failure on every real prompt. Two reference-design bugs and two harness gaps are in FINDINGS.md; I also filled leaderboard.json with real numbers (it is still dummy data in the upstream repo). The write-up lists the reruns I considered (16k tokens, Together AI for the Qwen anchor, Vitis for synthesis, CHStone/Rosetta) and did not do — harness frozen after the sweep, so the gaps stay attributable.

Analog (TRETS 2024 folded with JLPEA 2023): no pass@k table to match, so I re-derived the scaling anchors and the Fig 17–18 Case II CAB arithmetic — 48×128 VMM+WTA, 6,144 weights, 128 WTA CABs / 98 SoC CABs = 1.31 chips, which is the "barely more than one FPAA, fits in two" claim if one takes 98 from the SoC paper TRETS cites. I also ran Fig 16's acoustic program as a digital macromodel (16-band AmplitudeBPF → ladder delay → VMM+WTA) on yes/no/up/down: 70% held-out vs 25% chance. That is not a silicon number. The AnalogBenchmarks URL in TRETS footnote 1 was already 404 on Wayback in November 2023.

What I would want to do next in the group is AMS-Eval: the HLS-Eval metric ladder on your Fig 15 analog blocks (parseable → simulatable → functional → within CAB budget), using the macromodels from this run as the grader. I also read SmartPAF and CRYPTONITE — both look like clean numeric reproductions if the group cares about FHE or crypto HLS; I did not start them.

Happy to walk through any of it live.

Thanks,
Michael
