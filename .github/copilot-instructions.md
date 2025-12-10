# Paul Brain Modem Core - AI Coding Instructions

## Project Overview

This is a **MIL-STD-188-110A modem implementation** wrapped as a headless TCP server. The original modem core (`m188110a/`) was written by Paul Brain; the TCP wrapper (`src/main.cpp`) is by Phoenix Nest LLC for automated testing without audio hardware.

**Purpose**: Enable fair comparison testing between this modem and Phoenix Nest's implementation using identical TCP interfaces and PCM file formats.

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  brain_modem_server                     │
├─────────────────────────────────────────────────────────┤
│  Control Port (3999)  │  Data Port (3998)               │
│  ASCII commands       │  Binary TX/RX data              │
├─────────────────────────────────────────────────────────┤
│               src/main.cpp (TCP Wrapper)                │
│  - Socket handling, command parsing, PCM file I/O      │
│  - Global modem instance: g_modem (Cm110s class)       │
├─────────────────────────────────────────────────────────┤
│               m188110a/ (Paul Brain Core)               │
│  Cm110s.h    - Main modem class, Mode enum, callbacks  │
│  txm110a.cpp - TX modulation chain                     │
│  rxm110a.cpp - RX demodulation, DCD, sync              │
│  eq110a.cpp  - Adaptive equalizer (LMS)                │
│  de110a.cpp  - Deinterleaver, Viterbi decoder          │
│  in110a.cpp  - Interleaver                             │
│  v110a.cpp   - Viterbi encoder/decoder                 │
│  g110a.cpp   - Symbol generation tables                │
│  ptx110a.cpp - Preamble generation                     │
│  t110a.cpp   - TX filter, interpolation                │
└─────────────────────────────────────────────────────────┘
```

## Build & Run

```powershell
# Windows (MinGW g++ required, C++17)
.\build.ps1

# Linux/macOS
./build.sh

# Run server
.\brain_modem_server.exe   # Windows
./brain_modem_server       # Linux
```

**Key defines**: `-D_USE_MATH_DEFINES -DWIN32` (Windows) or just `-D_USE_MATH_DEFINES` (Linux)

## Data Flow

1. **TX**: Data port → `g_tx_buffer` → `CMD:SENDBUFFER` → `Cm110s::tx_sync_frame_eom()` → PCM samples → `tx_pcm_out/*.pcm`
2. **RX**: `CMD:RXAUDIOINJECT:<file>` → `Cm110s::rx_process_block()` → `rx_byte_callback()` → `g_rx_buffer` → Data port

## Critical Patterns

### Thread Safety
All modem operations use `g_modem_mutex`. RX buffer uses separate `g_rx_mutex`. Follow this pattern:
```cpp
std::lock_guard<std::mutex> lock(g_modem_mutex);
g_modem.tx_set_mode(mode);
```

### Mode Enumeration
Modes are `MxxxS` (short interleave) or `MxxxL` (long interleave). Use `string_to_mode()` and `mode_to_status_string()` for conversions. The `Mode` enum is in `Cm110s.h`.

### PCM Format
- Sample rate: 48kHz (server) internally resampled to 9600Hz (modem core)
- Format: 16-bit signed PCM, little-endian, mono
- Output directory: `./tx_pcm_out/`

### Command Protocol
Commands are `CMD:<verb>` or `CMD:QUERY:<item>`. Responses are single-line ASCII. Always handle via `handle_command()` in main.cpp.

## Modem Core Notes

- **Do not modify** `m188110a/*.cpp` files unless fixing bugs - this is Paul Brain's original code
- `platform_compat.h` provides Windows/Linux compatibility shims (Sleep, sprintf_s, gmtime_s)
- Modem internal sample rate is 9600 Hz (`M1_SAMPLE_RATE`), carrier frequency 1800 Hz (`M1_CENTER_FREQUENCY`)
- Soundblock size is configurable via `tx_set_soundblock_size()` (default 1024 samples)

## Testing

```bash
# Start server, then run test client
python test_client.py
```

The test client demonstrates the command/response protocol. PCM files appear in `tx_pcm_out/` for analysis.

## Adding New Commands

1. Add case in `handle_command()` function in `src/main.cpp`
2. Follow existing pattern: parse command, acquire mutex if touching modem, call modem API, send response
3. Prefix responses with `OK:`, `ERROR:`, or appropriate status prefix
