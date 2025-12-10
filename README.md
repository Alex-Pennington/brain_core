# Paul Brain Modem Core - Headless TCP Server

Wrapper for the original m188110a MIL-STD-188-110A modem core by Paul Brain.

Stripped of Qt/GUI dependencies for headless testing via TCP/IP and PCM files.

## Features

- **TCP Control Port (3999)**: ASCII commands for modem control
- **TCP Data Port (3998)**: Binary data TX/RX
- **PCM File Output**: TX audio saved to files for analysis
- **PCM File Inject**: Feed RX audio from files for testing
- **No Audio Devices**: Pure file-based operation for automated testing

## Building (Windows)

```powershell
.\build.ps1
```

Requires: MinGW g++ with C++17 support

## Usage

```powershell
.\brain_modem_server.exe
```

Server listens on:
- Control: `localhost:3999`
- Data: `localhost:3998`

## Commands (via Control Port)

| Command | Description |
|---------|-------------|
| `CMD:DATA RATE:<mode>` | Set mode (e.g., "600S", "1200L", "2400S") |
| `CMD:SENDBUFFER` | Transmit buffered data |
| `CMD:RESET MDM` | Reset modem state |
| `CMD:KILL TX` | Abort transmission |
| `CMD:RECORD TX:ON/OFF` | Enable/disable PCM recording |
| `CMD:RECORD PREFIX:<name>` | Set PCM filename prefix |
| `CMD:RXAUDIOINJECT:<path>` | Inject PCM file to RX |
| `CMD:QUERY:STATUS` | Get modem status |
| `CMD:QUERY:MODES` | List supported modes |
| `CMD:QUERY:VERSION` | Get version string |

## Supported Modes

- 75S, 75L (75 bps)
- 150S, 150L (150 bps)
- 300S, 300L (300 bps)
- 600S, 600L (600 bps)
- 1200S, 1200L (1200 bps)
- 2400S, 2400L (2400 bps)

(S = Short interleave, L = Long interleave)

## Testing Workflow

1. Start server
2. Connect control client to port 3999
3. Send data to port 3998
4. Send `CMD:DATA RATE:600S`
5. Send `CMD:SENDBUFFER`
6. PCM file appears in `./tx_pcm_out/`
7. Use `CMD:RXAUDIOINJECT:<pcm_file>` to test decode
8. Decoded data appears on port 3998

## Comparison Testing

This wrapper enables fair comparison between:
- Paul Brain modem core (this project)
- Phoenix Nest modem core

Both use identical:
- TCP interface (ports 3998/3999)
- Command set
- PCM file format (48kHz 16-bit)

## Credits

- Original modem core: Paul Brain (m188110a)
- TCP wrapper: Phoenix Nest LLC
- MIL-STD-188-110A specification: U.S. Department of Defense

## License

Original modem core license applies to m188110a/*.
Wrapper code (src/*) is GPL-3.0-or-later, Copyright (C) 2025 Phoenix Nest LLC.
