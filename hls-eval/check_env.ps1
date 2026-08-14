# check_env.ps1 - HLS-Eval reproduction environment probe
# Run in PowerShell:  powershell -ExecutionPolicy Bypass -File .\check_env.ps1
# Paste the whole output back into the chat.

$ErrorActionPreference = "SilentlyContinue"

function Section($t) { Write-Host ""; Write-Host "=== $t ===" -ForegroundColor Cyan }
function Ok($t)   { Write-Host "  [OK]   $t" -ForegroundColor Green }
function No($t)   { Write-Host "  [MISS] $t" -ForegroundColor Yellow }
function Info($t) { Write-Host "  $t" }

Write-Host "HLS-Eval environment probe" -ForegroundColor White
Write-Host ("Host: {0}   Date: {1}" -f $env:COMPUTERNAME, (Get-Date -Format "yyyy-MM-dd HH:mm"))

# ---------------------------------------------------------------- disk
Section "Disk space (need ~60 GB free for Vitis HLS in WSL)"
Get-CimInstance Win32_LogicalDisk -Filter "DriveType=3" | ForEach-Object {
    $free = [math]::Round($_.FreeSpace / 1GB, 1)
    $tot  = [math]::Round($_.Size / 1GB, 1)
    Info ("{0} {1} GB free of {2} GB" -f $_.DeviceID, $free, $tot)
}

# ---------------------------------------------------------------- WSL
Section "WSL"
$wslVer = wsl --version 2>&1 | Out-String
if ($LASTEXITCODE -eq 0 -and $wslVer.Trim()) {
    Ok "WSL installed"
    Info ($wslVer.Trim() -replace "`0", "")
} else {
    No "WSL not installed or not on PATH  ->  run: wsl --install"
}

$distros = (wsl -l -v 2>&1 | Out-String) -replace "`0", ""
if ($distros.Trim()) {
    Info "Distros:"
    Info $distros.Trim()
} else {
    No "No WSL distros registered"
}

# ---------------------------------------------------------------- Vitis on Windows
Section "Vitis HLS on Windows"
$vhlsWin = Get-Command vitis_hls -ErrorAction SilentlyContinue
if ($vhlsWin) { Ok "vitis_hls on PATH: $($vhlsWin.Source)" } else { No "vitis_hls not on Windows PATH" }

$xilinxRoots = @("C:\Xilinx", "D:\Xilinx", "C:\Program Files\Xilinx", "C:\tools\Xilinx", "C:\AMD", "D:\AMD")
$foundAny = $false
foreach ($root in $xilinxRoots) {
    if (Test-Path $root) {
        $foundAny = $true
        Ok "Install root exists: $root"
        Get-ChildItem $root -Directory | ForEach-Object {
            Info "    $($_.Name)"
            Get-ChildItem $_.FullName -Directory -ErrorAction SilentlyContinue |
                Select-Object -First 6 | ForEach-Object { Info "      - $($_.Name)" }
        }
    }
}
if (-not $foundAny) { No "No Xilinx/AMD install directory found in common locations" }

# ---------------------------------------------------------------- Vitis in WSL
Section "Vitis HLS inside WSL"
$wslProbe = @'
echo "distro: $(. /etc/os-release 2>/dev/null; echo $PRETTY_NAME)"
echo "home-free: $(df -h ~ | tail -1 | awk '{print $4}')"
if command -v vitis_hls >/dev/null 2>&1; then
  echo "vitis_hls: FOUND at $(command -v vitis_hls)"
else
  echo "vitis_hls: NOT on PATH"
fi
for d in /tools/Xilinx /opt/Xilinx /tools/xilinx /mnt/c/Xilinx ~/Xilinx; do
  [ -d "$d" ] && echo "xilinx-dir: $d -> $(ls "$d" 2>/dev/null | head -4 | tr '\n' ' ')"
done
echo "python3: $(python3 --version 2>&1)"
echo "uv: $(command -v uv >/dev/null 2>&1 && uv --version || echo MISSING)"
echo "git: $(git --version 2>&1)"
echo "cpu-cores: $(nproc)"
echo "ram-gb: $(free -g | awk '/^Mem:/{print $2}')"
'@
$wslOut = $wslProbe | wsl bash -s 2>&1 | Out-String
if ($wslOut.Trim()) { Info ($wslOut.Trim() -replace "`0", "") } else { No "Could not run commands in WSL" }

# ---------------------------------------------------------------- Windows toolchain
Section "Windows toolchain"
foreach ($t in @("python", "git", "uv", "nvidia-smi")) {
    $c = Get-Command $t -ErrorAction SilentlyContinue
    if ($c) {
        $v = (& $t --version 2>&1 | Select-Object -First 1) -join " "
        Ok "$t  ->  $v"
    } else { No "$t not found" }
}

Section "GPU"
$gpu = nvidia-smi --query-gpu=name,memory.total,driver_version --format=csv,noheader 2>&1
if ($LASTEXITCODE -eq 0) { Info $gpu } else { No "nvidia-smi unavailable (fine - not required for this project)" }

Write-Host ""
Write-Host "Done. Copy everything above back into the chat." -ForegroundColor White
