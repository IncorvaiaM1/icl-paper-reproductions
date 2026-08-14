# CRYPTONITE (parked)

*Scalable Accelerator Design for Cryptographic Primitives and Algorithms* —
arXiv 2505.14657 / IEEE. Code: https://github.com/KarthikeyaSharma16/Cryptonite

Auto-generates HLS accelerators for elliptic-curve crypto (Fiat Cryptography
primitives). Claims 88.88% resource reduction / 54.31% latency improvement vs
naive synthesis.

## Why not now

Real GitHub, concrete numbers, but:

- Requires Vitis HLS 2023.1 + a specific Xilinx board + SVF/GiNaC/egg/LLVM
- HLS-Eval reproduction went Vitis-free specifically to avoid this dependency
- Not AI/NN-flavored; weaker fit for the "I want to work on LLM-for-hardware
  and analog/neuromorphic" pitch

Revisit only if there is a specific reason to want a crypto-accelerator project.
