# build_lib.ps1 - Build static library from source (Windows)
# Usage: .\build_lib.ps1

$ErrorActionPreference = "Stop"

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Building libm188110a.a (Windows x64)" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

# Check source exists
if (-not (Test-Path "m188110a")) {
    Write-Host "ERROR: m188110a/ source directory not found!" -ForegroundColor Red
    Write-Host "This directory should contain the proprietary modem core source." -ForegroundColor Yellow
    exit 1
}

$sources = @(
    "m188110a/de110a.cpp",
    "m188110a/eq110a.cpp",
    "m188110a/g110a.cpp",
    "m188110a/in110a.cpp",
    "m188110a/ptx110a.cpp",
    "m188110a/rxm110a.cpp",
    "m188110a/t110a.cpp",
    "m188110a/txm110a.cpp",
    "m188110a/v110a.cpp"
)

# Verify all sources exist
foreach ($src in $sources) {
    if (-not (Test-Path $src)) {
        Write-Host "ERROR: Missing source file: $src" -ForegroundColor Red
        exit 1
    }
}

# Create lib directory
New-Item -ItemType Directory -Path "lib/win64" -Force | Out-Null

Write-Host "`nCompiling object files..." -ForegroundColor Yellow

# Compile each source to object file
$objects = @()
foreach ($src in $sources) {
    $obj = [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
    $objects += $obj
    Write-Host "  $src -> $obj" -ForegroundColor DarkGray
    $cmd = "g++ -c -std=c++17 -O2 -Iinclude/m188110a -D_USE_MATH_DEFINES -DWIN32 -o $obj $src"
    Invoke-Expression $cmd
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Failed to compile $src" -ForegroundColor Red
        exit 1
    }
}

Write-Host "`nCreating static library..." -ForegroundColor Yellow
$objList = $objects -join " "
$cmd = "ar rcs lib/win64/libm188110a.a $objList"
Write-Host "  $cmd" -ForegroundColor DarkGray
Invoke-Expression $cmd

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to create library" -ForegroundColor Red
    exit 1
}

# Clean up object files
Write-Host "`nCleaning up..." -ForegroundColor Yellow
foreach ($obj in $objects) {
    Remove-Item $obj -Force -ErrorAction SilentlyContinue
}

$size = (Get-Item "lib/win64/libm188110a.a").Length / 1KB
Write-Host "`n============================================" -ForegroundColor Cyan
Write-Host "Library built: lib/win64/libm188110a.a" -ForegroundColor Green
Write-Host "Size: $([math]::Round($size, 1)) KB" -ForegroundColor Gray
Write-Host "============================================" -ForegroundColor Cyan
