#!/usr/bin/env bash
# Bootstrap the HLS-Eval reproduction environment inside WSL/Ubuntu.
# Idempotent - safe to re-run.
#
#   wsl
#   cd /mnt/c/Users/micha/MAIStuff/randLab/hls-eval
#   bash scripts/bootstrap_wsl.sh

set -euo pipefail

WORKROOT="${WORKROOT:-$HOME/randlab}"
REPO_DIR="$WORKROOT/hls-eval"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

bold() { printf '\033[1m%s\033[0m\n' "$*"; }
ok()   { printf '  \033[32m[ok]\033[0m %s\n' "$*"; }
warn() { printf '  \033[33m[!]\033[0m %s\n' "$*"; }
step() { printf '\n\033[36m==> %s\033[0m\n' "$*"; }

bold "HLS-Eval reproduction bootstrap"
echo "project : $PROJECT_DIR"
echo "workroot: $WORKROOT"

# ----------------------------------------------------------------- sanity
step "Environment"
if ! grep -qi microsoft /proc/version 2>/dev/null; then
  warn "not detected as WSL - continuing, but paths assume a Linux host"
fi
echo "  distro: $(. /etc/os-release && echo "$PRETTY_NAME")"
echo "  cores : $(nproc)"
echo "  ram   : $(free -g | awk '/^Mem:/{print $2}') GB"
echo "  free  : $(df -h "$HOME" | tail -1 | awk '{print $4}') on \$HOME"

# ----------------------------------------------------------------- packages
step "System packages"
NEED=()
for pkg in clang build-essential git curl python3 python3-venv; do
  dpkg -s "$pkg" >/dev/null 2>&1 || NEED+=("$pkg")
done
if [ ${#NEED[@]} -gt 0 ]; then
  echo "  installing: ${NEED[*]}"
  sudo apt-get update -qq
  sudo apt-get install -y -qq "${NEED[@]}"
else
  ok "all present"
fi
echo "  $(clang++ --version | head -1)"

# ----------------------------------------------------------------- uv
step "uv"
if ! command -v uv >/dev/null 2>&1; then
  curl -LsSf https://astral.sh/uv/install.sh | sh
  export PATH="$HOME/.local/bin:$PATH"
  grep -q '.local/bin' "$HOME/.bashrc" 2>/dev/null || \
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
fi
ok "$(uv --version)"

# ----------------------------------------------------------------- repo
step "hls-eval repository"
mkdir -p "$WORKROOT"
if [ -d "$REPO_DIR/.git" ]; then
  ok "already cloned at $REPO_DIR"
  git -C "$REPO_DIR" pull --ff-only 2>/dev/null || warn "could not fast-forward; leaving as-is"
else
  git clone https://github.com/sharc-lab/hls-eval.git "$REPO_DIR"
fi
echo "  HEAD: $(git -C "$REPO_DIR" rev-parse --short HEAD)  $(git -C "$REPO_DIR" log -1 --format=%cd --date=short)"

step "Python dependencies (uv sync)"
( cd "$REPO_DIR" && uv sync )
ok "virtualenv at $REPO_DIR/.venv"

# ----------------------------------------------------------------- wire in ext/
step "Linking project extensions into the repo"
ln -sfn "$PROJECT_DIR/ext" "$REPO_DIR/ext"
ok "$REPO_DIR/ext -> $PROJECT_DIR/ext"

# ----------------------------------------------------------------- vitis?
step "Vitis HLS (optional - only needed for the synthesizability metric)"
if command -v vitis_hls >/dev/null 2>&1; then
  ok "found: $(command -v vitis_hls)"
  echo "  -> all four metrics available"
else
  warn "not on PATH - metrics 1-3 (parse/compile/run) still work"
  for d in /tools/Xilinx /opt/Xilinx "$HOME/Xilinx"; do
    [ -d "$d" ] && echo "     candidate install dir: $d ($(ls "$d" 2>/dev/null | head -3 | tr '\n' ' '))"
  done
fi

# ----------------------------------------------------------------- validate
step "Validating the Vitis-free backend against reference designs"
python3 "$PROJECT_DIR/scripts/verify_backend.py" \
  --data "$REPO_DIR/hls_eval_data" \
  --suite polybench --suite machsuite --suite c2hlsc \
  --jobs "$(nproc)" \
  --json "$PROJECT_DIR/results/backend_check.json" | tail -20

# ----------------------------------------------------------------- api key
step "API key"
if [ -f "$REPO_DIR/.env" ] && grep -q OPENROUTER_API_KEY "$REPO_DIR/.env"; then
  ok ".env has OPENROUTER_API_KEY"
else
  warn "no OPENROUTER_API_KEY in $REPO_DIR/.env"
  echo "     get a key at https://openrouter.ai/keys then:"
  echo "     echo 'OPENROUTER_API_KEY=sk-or-...' >> $REPO_DIR/.env"
fi

bold "
Done.

Next:
  cd $REPO_DIR
  uv run python $PROJECT_DIR/scripts/run_experiment.py --smoke
"
