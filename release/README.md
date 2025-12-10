# Paul Brain Modem Core - Headless TCP Server
Version 1.0.0

## Quick Start

1. Create a license.key file with your license key (contact Phoenix Nest LLC)
2. Run: brain_modem_server.exe
3. Connect to:
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
