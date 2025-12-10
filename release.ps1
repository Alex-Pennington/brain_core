# release.ps1 - Build and package release for distribution
# Usage: .\release.ps1

$ErrorActionPreference = "Stop"

$Version = "1.0.0"
$ReleaseName = "brain_modem_server_v$Version"
$ReleaseDir = "release"
$ZipFile = "$ReleaseName.zip"

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "Paul Brain Modem Core - Release Build" -ForegroundColor Cyan
Write-Host "Version: $Version" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

# Step 1: Build
Write-Host "`n[1/4] Building..." -ForegroundColor Yellow
.\build.ps1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Step 2: Create release directory
Write-Host "`n[2/4] Creating release directory..." -ForegroundColor Yellow
if (Test-Path $ReleaseDir) {
    Remove-Item -Recurse -Force $ReleaseDir
}
New-Item -ItemType Directory -Path $ReleaseDir | Out-Null
New-Item -ItemType Directory -Path "$ReleaseDir\tx_pcm_out" | Out-Null

# Step 3: Copy files
Write-Host "`n[3/4] Copying files..." -ForegroundColor Yellow

# Executable
Copy-Item "brain_modem_server.exe" "$ReleaseDir\"
Write-Host "  - brain_modem_server.exe" -ForegroundColor Gray



# README
$readme = @"
# Paul Brain Modem Core - Headless TCP Server
Version $Version

## Quick Start

1. Run: brain_modem_server.exe
2. Connect to:
   - Control port: localhost:3999
   - Data port: localhost:3998

## Commands

| Command | Description |
|---------|-------------|
| CMD:DATA RATE:600S | Set mode (75S/L, 150S/L, 300S/L, 600S/L, 1200S/L, 2400S/L, 4800S) |
| CMD:SENDBUFFER | Transmit buffered data |
| CMD:RXAUDIOINJECT:path.pcm | Inject PCM file for RX decode |
| CMD:QUERY:STATUS | Get current status |

## Data Flow

### Transmit (TX)
1. Connect to data port 3998
2. Send binary data
3. Send CMD:SENDBUFFER on control port
4. PCM file generated in tx_pcm_out/

### Receive (RX)  
1. Send CMD:RXAUDIOINJECT:filepath.pcm on control port
2. Decoded data appears on data port 3998

## PCM Format
- Sample rate: 9600 Hz
- Format: 16-bit signed, little-endian, mono

## Requirements
- Windows 10 or later
- No additional DLLs required

## Support
Contact: Phoenix Nest LLC
"@
$readme | Out-File -FilePath "$ReleaseDir\README.md" -Encoding UTF8
Write-Host "  - README.md" -ForegroundColor Gray

# Step 4: Create ZIP
Write-Host "`n[4/4] Creating ZIP archive..." -ForegroundColor Yellow
if (Test-Path $ZipFile) {
    Remove-Item $ZipFile
}
Compress-Archive -Path "$ReleaseDir\*" -DestinationPath $ZipFile -CompressionLevel Optimal

$zipSize = [math]::Round((Get-Item $ZipFile).Length / 1MB, 2)

Write-Host "`n============================================" -ForegroundColor Green
Write-Host "Release build complete!" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host ""
Write-Host "Output: $ZipFile ($zipSize MB)" -ForegroundColor White
Write-Host ""
Write-Host "Contents:" -ForegroundColor Cyan
Get-ChildItem "$ReleaseDir" -Recurse | ForEach-Object {
    $relativePath = $_.FullName.Replace((Get-Item $ReleaseDir).FullName, "").TrimStart("\")
    if ($_.PSIsContainer) {
        Write-Host "  [DIR] $relativePath\" -ForegroundColor Gray
    } else {
        $size = [math]::Round($_.Length / 1KB, 1)
        Write-Host "  $relativePath ($size KB)" -ForegroundColor Gray
    }
}
Write-Host ""
Write-Host "Ready for distribution!" -ForegroundColor Green
