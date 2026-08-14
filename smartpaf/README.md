# SmartPAF (parked)

Tong, Dang, Golder, Raychowdhury, Hao, Krishna — *Accurate Low-Degree Polynomial
Approximation of Non-Polynomial Operators for Fast Private Inference in
Homomorphic Encryption*, MLSys 2024.

- Proceedings: https://proceedings.mlsys.org/paper_files/paper/2024/hash/621d0fd41c720ab252e178b77c200d90-Abstract-Conference.html
- Anonymized code (may 404): https://anonymous.4open.science/r/SmartPAF-64E1
- Search for a de-anonymized GitHub after publication before treating this as blocked.

## Claims worth reproducing (later)

Replaces ReLU/softmax/etc. with tuned low-degree polynomials for FHE inference.
1.42–13.64× accuracy improvement and 6.79–14.9× speedup vs prior work on
ResNet-18 / ImageNet-1k.

## Why not now

Closest analog to the HLS-Eval playbook (public code, numeric claims, standard
benchmark) but **not analog/neuromorphic** and ImageNet+FHE is a multi-day
environment lift. The lab email is about HLS-Eval + the Hasler/Hao analog
bridge. Starting this in the same 48 hours splits the story.

Revisit if Callie asks about FHE/private inference, or after analog Case I ships.
