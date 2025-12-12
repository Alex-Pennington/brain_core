# build.ps1 - Paul Brain Modem Core Build Script

$ErrorActionPreference = "Stop"

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Paul Brain Modem Core - Build" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

# Generate version string with git info
$commitShort = "unknown"
try {
    $commitShort = (git rev-parse --short HEAD 2>$null)
    if (-not $commitShort) { $commitShort = "unknown" }
} catch { }
$version = "v1.0.0-$commitShort"
Write-Host "Version: $version" -ForegroundColor Gray

# Create version header
$versionHeader = @"
#ifndef VERSION_H
#define VERSION_H
#define VERSION_STRING "$version"
#endif
"@
$versionHeader | Out-File -FilePath (Join-Path $PSScriptRoot "src\version.h") -Encoding ASCII

$CXX = "g++"
$CXXFLAGS = "-std=c++17 -O2 -I. -Iinclude -Iinclude/m188110a -D_USE_MATH_DEFINES -DWIN32"
$LDFLAGS = "-Llib/win64 -lm188110a -lws2_32 -static -static-libgcc -static-libstdc++"

$Sources = @("src/main.cpp")
$Output = "brain_modem_server.exe"

Write-Host ""
Write-Host "Compiling..." -ForegroundColor Yellow

$cmd = "$CXX $CXXFLAGS -o $Output $($Sources -join ' ') $LDFLAGS"
Write-Host $cmd -ForegroundColor DarkGray
Invoke-Expression $cmd

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

$size = [math]::Round((Get-Item $Output).Length / 1KB, 1)
Write-Host "Build successful: $Output" -ForegroundColor Green
Write-Host "Size: $size KB" -ForegroundColor Gray

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Usage:" -ForegroundColor White
Write-Host "  .\brain_modem_server.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "Connect to:" -ForegroundColor White
Write-Host "  Control: localhost:3999" -ForegroundColor Gray
Write-Host "  Data:    localhost:3998" -ForegroundColor Gray
Write-Host "============================================" -ForegroundColor Cyan
