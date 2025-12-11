# build.ps1 - Paul Brain Modem Core Build Script
# Usage: .\build.ps1 [-Increment patch|minor|major]

param(
    [ValidateSet("patch", "minor", "major")]
    [string]$Increment
)

$ErrorActionPreference = "Stop"

# Read current version from VERSION file
$versionFile = Join-Path $PSScriptRoot "VERSION"
if (Test-Path $versionFile) {
    $baseVersion = (Get-Content $versionFile -Raw).Trim()
} else {
    $baseVersion = "1.0.0"
    $baseVersion | Out-File -FilePath $versionFile -Encoding ASCII -NoNewline
}

# Parse version components
if ($baseVersion -match '^(\d+)\.(\d+)\.(\d+)$') {
    $major = [int]$Matches[1]
    $minor = [int]$Matches[2]
    $patch = [int]$Matches[3]
} else {
    Write-Host "Invalid version format in VERSION file: $baseVersion" -ForegroundColor Red
    exit 1
}

# Increment version if requested
if ($Increment) {
    switch ($Increment) {
        "major" { $major++; $minor = 0; $patch = 0 }
        "minor" { $minor++; $patch = 0 }
        "patch" { $patch++ }
    }
    $baseVersion = "$major.$minor.$patch"
    $baseVersion | Out-File -FilePath $versionFile -Encoding ASCII -NoNewline
    Write-Host "Version incremented to: $baseVersion" -ForegroundColor Yellow
}

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Paul Brain Modem Core - Build" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

# Generate local version string with git info
$commitShort = "unknown"
try {
    $commitShort = (git rev-parse --short HEAD 2>$null)
    if (-not $commitShort) { $commitShort = "unknown" }
} catch { }
$localVersion = "v$baseVersion-local-$commitShort"
Write-Host "Version: $localVersion" -ForegroundColor Gray

$CXX = "g++"
$CXXFLAGS = "-std=c++17 -O2 -I. -Iinclude -Iinclude/m188110a -D_USE_MATH_DEFINES -DWIN32"
# Static link libgcc and libstdc++ - KERNEL32/WS2_32/UCRT are Windows system DLLs
$LDFLAGS = "-Llib/win64 -lm188110a -lws2_32 -static -static-libgcc -static-libstdc++"

$Sources = @(
    "src/main.cpp"
)

$Output = "brain_modem_server.exe"

# Create output directory
if (-not (Test-Path "tx_pcm_out")) {
    New-Item -ItemType Directory -Path "tx_pcm_out" | Out-Null
}

# Generate version header
$versionHeader = @"
// Auto-generated version header
#ifndef VERSION_H
#define VERSION_H
#define VERSION_STRING "$localVersion"
#endif
"@
$versionHeader | Out-File -FilePath "src/version.h" -Encoding ASCII -NoNewline

Write-Host "`nCompiling..." -ForegroundColor Yellow

$sourceList = $Sources -join ' '
$cmd = "$CXX $CXXFLAGS -o $Output $sourceList $LDFLAGS"
Write-Host $cmd -ForegroundColor DarkGray

try {
    Invoke-Expression "& $cmd 2>&1"
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed"
    }
    Write-Host "`nBuild successful: $Output" -ForegroundColor Green
    
    $size = (Get-Item $Output).Length / 1KB
    Write-Host "Size: $([math]::Round($size, 1)) KB" -ForegroundColor Gray
} catch {
    Write-Host "`nBuild failed: $_" -ForegroundColor Red
    exit 1
}

Write-Host "`n============================================" -ForegroundColor Cyan
Write-Host "Usage:" -ForegroundColor Cyan
Write-Host "  .\brain_modem_server.exe" -ForegroundColor White
Write-Host "" -ForegroundColor White
Write-Host "Connect to:" -ForegroundColor Cyan
Write-Host "  Control: localhost:3999" -ForegroundColor White
Write-Host "  Data:    localhost:3998" -ForegroundColor White
Write-Host "" -ForegroundColor White
Write-Host "Commands:" -ForegroundColor Cyan  
Write-Host "  CMD:DATA RATE:600S" -ForegroundColor White
Write-Host "  CMD:SENDBUFFER" -ForegroundColor White
Write-Host "  CMD:RXAUDIOINJECT:path/to/file.pcm" -ForegroundColor White
Write-Host "  CMD:QUERY:STATUS" -ForegroundColor White
Write-Host "============================================" -ForegroundColor Cyan
