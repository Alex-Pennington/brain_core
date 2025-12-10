#!/usr/bin/env python3
"""
Compare TX output between Paul Brain modem and Phoenix Nest modem.

Connects to both modems, generates TX for identical test message,
and performs detailed sample-by-sample and symbol-by-symbol comparison.

Brain modem: Control 3999, Data 3998
Phoenix Nest: Control 5100, Data 5101
"""

import socket
import time
import struct
import numpy as np
import os
from pathlib import Path

# Try to import matplotlib, but continue without plots if not available
try:
    import matplotlib.pyplot as plt
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False
    print("Warning: matplotlib not available, skipping plots")

# Modem connection settings
BRAIN_CONTROL = ('127.0.0.1', 3999)
BRAIN_DATA = ('127.0.0.1', 3998)
PHOENIX_CONTROL = ('127.0.0.1', 4999)
PHOENIX_DATA = ('127.0.0.1', 4998)

# Modem parameters
SAMPLE_RATE = 9600  # Hz
CARRIER_FREQ = 1800  # Hz
SYMBOL_RATE = 1600   # symbols/sec for 600 bps mode
SAMPLES_PER_SYMBOL = SAMPLE_RATE // SYMBOL_RATE

# Test configuration
TEST_MODE = "600S"
TEST_MESSAGE = b"HELLO MODEM COMPARISON TEST " + b"A" * 50  # 79 bytes


def connect_modem(name, control_addr, data_addr):
    """Connect to a modem's control and data ports."""
    print(f"\n[{name}] Connecting to {control_addr[0]}:{control_addr[1]}...")
    
    ctrl = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ctrl.settimeout(5.0)
    try:
        ctrl.connect(control_addr)
    except ConnectionRefusedError:
        print(f"[{name}] ERROR: Connection refused. Is the server running?")
        return None, None
    
    # Drain welcome message
    try:
        welcome = ctrl.recv(1024).decode().strip()
        print(f"[{name}] {welcome}")
    except:
        pass
    
    data = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    data.settimeout(5.0)
    data.connect(data_addr)
    print(f"[{name}] Connected to data port")
    
    return ctrl, data


def send_cmd(sock, cmd, timeout=2.0):
    """Send command and get response."""
    sock.sendall((cmd + '\n').encode())
    time.sleep(0.1)
    sock.settimeout(timeout)
    try:
        return sock.recv(4096).decode().strip()
    except socket.timeout:
        return None


def drain_responses(sock, timeout=0.5):
    """Drain all pending responses."""
    responses = []
    sock.settimeout(timeout)
    while True:
        try:
            data = sock.recv(4096).decode().strip()
            if data:
                responses.extend(data.split('\n'))
        except socket.timeout:
            break
    return responses


def generate_tx(name, ctrl, data, mode, message):
    """Generate TX and return PCM file path."""
    print(f"\n[{name}] Generating TX...")
    
    # Set mode
    resp = send_cmd(ctrl, f"CMD:DATA RATE:{mode}")
    print(f"[{name}] Mode: {resp}")
    
    # Send data
    data.sendall(message)
    print(f"[{name}] Sent {len(message)} bytes")
    time.sleep(0.3)
    
    # Trigger TX
    ctrl.sendall(b"CMD:SENDBUFFER\n")
    
    # Wait for TX to complete
    print(f"[{name}] Waiting for TX...")
    time.sleep(6)
    
    # Get responses
    responses = drain_responses(ctrl, timeout=2.0)
    
    pcm_file = None
    for r in responses:
        print(f"[{name}] {r}")
        if "TX:PCM:" in r:
            pcm_file = r.split("TX:PCM:")[1]
        elif "PCM:" in r:
            pcm_file = r.split("PCM:")[1]
    
    return pcm_file


def load_pcm_file(filepath):
    """Load PCM file as numpy array of floats."""
    if not os.path.exists(filepath):
        print(f"ERROR: File not found: {filepath}")
        return None
    
    with open(filepath, 'rb') as f:
        raw = f.read()
    
    # Assume 16-bit signed PCM
    samples = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
    print(f"Loaded {len(samples)} samples from {filepath}")
    return samples


def cross_correlate(a, b, max_lag=1000):
    """Find lag between two signals using cross-correlation."""
    if len(a) < max_lag or len(b) < max_lag:
        max_lag = min(len(a), len(b)) // 2
    
    # Use a shorter segment for correlation
    seg_len = min(len(a), len(b), 10000)
    seg_a = a[:seg_len]
    seg_b = b[:seg_len]
    
    best_lag = 0
    best_corr = 0
    
    for lag in range(-max_lag, max_lag + 1):
        if lag < 0:
            corr = np.sum(seg_a[-lag:] * seg_b[:len(seg_a)+lag])
        else:
            corr = np.sum(seg_a[:len(seg_a)-lag] * seg_b[lag:])
        
        if corr > best_corr:
            best_corr = corr
            best_lag = lag
    
    return best_lag, best_corr


def extract_symbols(samples, carrier_freq=CARRIER_FREQ, sample_rate=SAMPLE_RATE, 
                   symbol_rate=SYMBOL_RATE):
    """Extract complex symbols from PSK signal."""
    t = np.arange(len(samples)) / sample_rate
    
    # Downconvert to baseband
    i_carrier = np.cos(2 * np.pi * carrier_freq * t)
    q_carrier = np.sin(2 * np.pi * carrier_freq * t)
    
    i_baseband = samples * i_carrier
    q_baseband = samples * q_carrier
    
    # Low-pass filter (simple moving average)
    filter_len = sample_rate // symbol_rate
    i_filtered = np.convolve(i_baseband, np.ones(filter_len)/filter_len, mode='same')
    q_filtered = np.convolve(q_baseband, np.ones(filter_len)/filter_len, mode='same')
    
    # Sample at symbol rate
    samples_per_symbol = sample_rate // symbol_rate
    symbol_indices = np.arange(0, len(samples) - samples_per_symbol, samples_per_symbol)
    
    symbols = i_filtered[symbol_indices] + 1j * q_filtered[symbol_indices]
    
    return symbols


def compare_signals(brain_samples, phoenix_samples):
    """Compare two PCM signals in detail."""
    print("\n" + "="*60)
    print("SIGNAL COMPARISON")
    print("="*60)
    
    # Basic stats
    print(f"\nLength: Brain={len(brain_samples)}, Phoenix={len(phoenix_samples)}")
    print(f"Difference: {len(brain_samples) - len(phoenix_samples)} samples "
          f"({(len(brain_samples) - len(phoenix_samples))/SAMPLE_RATE*1000:.1f} ms)")
    
    brain_rms = np.sqrt(np.mean(brain_samples**2))
    phoenix_rms = np.sqrt(np.mean(phoenix_samples**2))
    print(f"RMS: Brain={brain_rms:.4f}, Phoenix={phoenix_rms:.4f}")
    
    # Cross-correlation to find alignment
    lag, corr = cross_correlate(brain_samples, phoenix_samples)
    print(f"\nBest alignment lag: {lag} samples ({lag/SAMPLE_RATE*1000:.2f} ms)")
    
    # Align signals
    if lag > 0:
        aligned_brain = brain_samples[lag:]
        aligned_phoenix = phoenix_samples[:len(aligned_brain)]
    else:
        aligned_phoenix = phoenix_samples[-lag:]
        aligned_brain = brain_samples[:len(aligned_phoenix)]
    
    min_len = min(len(aligned_brain), len(aligned_phoenix))
    aligned_brain = aligned_brain[:min_len]
    aligned_phoenix = aligned_phoenix[:min_len]
    
    # Sample-by-sample difference
    diff = aligned_brain - aligned_phoenix
    diff_rms = np.sqrt(np.mean(diff**2))
    max_diff = np.max(np.abs(diff))
    print(f"\nAligned difference RMS: {diff_rms:.6f}")
    print(f"Max difference: {max_diff:.6f}")
    
    # Find first significant difference
    threshold = 0.01
    diff_indices = np.where(np.abs(diff) > threshold)[0]
    if len(diff_indices) > 0:
        first_diff_sample = diff_indices[0]
        first_diff_time = first_diff_sample / SAMPLE_RATE
        print(f"\nFirst difference > {threshold}: sample {first_diff_sample} "
              f"(t={first_diff_time*1000:.2f} ms)")
    else:
        print(f"\nNo differences > {threshold} found!")
    
    return aligned_brain, aligned_phoenix, lag


def compare_symbols(brain_samples, phoenix_samples):
    """Compare extracted symbols."""
    print("\n" + "="*60)
    print("SYMBOL COMPARISON")
    print("="*60)
    
    brain_symbols = extract_symbols(brain_samples)
    phoenix_symbols = extract_symbols(phoenix_samples)
    
    print(f"\nSymbols: Brain={len(brain_symbols)}, Phoenix={len(phoenix_symbols)}")
    
    min_len = min(len(brain_symbols), len(phoenix_symbols))
    brain_symbols = brain_symbols[:min_len]
    phoenix_symbols = phoenix_symbols[:min_len]
    
    # Compare phases
    brain_phase = np.angle(brain_symbols)
    phoenix_phase = np.angle(phoenix_symbols)
    
    # Unwrap phases
    brain_phase = np.unwrap(brain_phase)
    phoenix_phase = np.unwrap(phoenix_phase)
    
    # Phase difference
    phase_diff = brain_phase - phoenix_phase
    
    # Remove linear trend (frequency offset)
    x = np.arange(len(phase_diff))
    coeffs = np.polyfit(x, phase_diff, 1)
    trend = np.polyval(coeffs, x)
    phase_diff_detrended = phase_diff - trend
    
    print(f"Phase trend: {coeffs[0]*1000:.4f} mrad/symbol (freq offset)")
    print(f"Mean phase diff: {np.mean(phase_diff_detrended):.4f} rad")
    print(f"Std phase diff: {np.std(phase_diff_detrended):.4f} rad")
    
    # Find first symbol mismatch
    phase_threshold = 0.5  # radians (~30 degrees)
    mismatch_indices = np.where(np.abs(phase_diff_detrended) > phase_threshold)[0]
    
    if len(mismatch_indices) > 0:
        first_mismatch = mismatch_indices[0]
        print(f"\nFirst phase mismatch > {phase_threshold:.1f} rad: symbol {first_mismatch}")
        
        # Estimate where this is in the frame
        preamble_symbols = int(0.6 * SYMBOL_RATE)  # ~600ms preamble
        if first_mismatch < preamble_symbols:
            print(f"  -> In PREAMBLE region (symbol {first_mismatch}/{preamble_symbols})")
        else:
            print(f"  -> In DATA region (symbol {first_mismatch - preamble_symbols} after preamble)")
    else:
        print(f"\nNo phase mismatches > {phase_threshold:.1f} rad found!")
    
    return brain_symbols, phoenix_symbols, phase_diff_detrended


def analyze_preamble(samples, name):
    """Analyze preamble structure looking for D1/D2 patterns."""
    print(f"\n[{name}] Preamble Analysis")
    
    # Extract first 1 second (preamble region)
    preamble_samples = samples[:SAMPLE_RATE]
    
    symbols = extract_symbols(preamble_samples)
    phases = np.angle(symbols)
    
    # Look for phase reversals (D1/D2 pattern is alternating)
    phase_diffs = np.diff(phases)
    
    # Normalize to [-pi, pi]
    phase_diffs = np.mod(phase_diffs + np.pi, 2*np.pi) - np.pi
    
    # Find reversals (phase change close to pi)
    reversals = np.abs(np.abs(phase_diffs) - np.pi) < 0.5
    reversal_count = np.sum(reversals)
    
    print(f"  Phase reversals in first 1s: {reversal_count}")
    print(f"  First 20 phase diffs: {phase_diffs[:20]}")
    
    return symbols, phases


def generate_plots(brain_samples, phoenix_samples, brain_symbols, phoenix_symbols,
                  phase_diff, lag):
    """Generate comparison plots."""
    if not HAS_MATPLOTLIB:
        print("\nSkipping plots (matplotlib not available)")
        return
    
    fig, axes = plt.subplots(3, 2, figsize=(14, 10))
    fig.suptitle('Brain vs Phoenix Nest TX Comparison', fontsize=14)
    
    # 1. Waveform overlay (first 50ms)
    ax = axes[0, 0]
    t_ms = np.arange(min(500, len(brain_samples))) / SAMPLE_RATE * 1000
    ax.plot(t_ms, brain_samples[:len(t_ms)], 'b-', alpha=0.7, label='Brain')
    ax.plot(t_ms, phoenix_samples[:len(t_ms)], 'r-', alpha=0.7, label='Phoenix')
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Amplitude')
    ax.set_title('Waveform (first 50ms)')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # 2. Zoomed preamble (carrier onset)
    ax = axes[0, 1]
    # Find where signal starts
    energy = np.convolve(brain_samples**2, np.ones(100)/100, mode='same')
    onset = np.argmax(energy > 0.001 * np.max(energy))
    t_start = max(0, onset - 100)
    t_end = min(len(brain_samples), onset + 500)
    t_ms = (np.arange(t_start, t_end) - onset) / SAMPLE_RATE * 1000
    ax.plot(t_ms, brain_samples[t_start:t_end], 'b-', alpha=0.7, label='Brain')
    if t_end <= len(phoenix_samples):
        ax.plot(t_ms, phoenix_samples[t_start:t_end], 'r-', alpha=0.7, label='Phoenix')
    ax.set_xlabel('Time from onset (ms)')
    ax.set_ylabel('Amplitude')
    ax.set_title('Carrier Onset')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # 3. Power spectrum
    ax = axes[1, 0]
    fft_len = 4096
    freqs = np.fft.fftfreq(fft_len, 1/SAMPLE_RATE)[:fft_len//2]
    
    brain_fft = np.abs(np.fft.fft(brain_samples[:fft_len]))[:fft_len//2]
    phoenix_fft = np.abs(np.fft.fft(phoenix_samples[:min(fft_len, len(phoenix_samples))]))
    phoenix_fft = phoenix_fft[:len(phoenix_fft)//2]
    
    ax.semilogy(freqs, brain_fft, 'b-', alpha=0.7, label='Brain')
    ax.semilogy(freqs[:len(phoenix_fft)], phoenix_fft, 'r-', alpha=0.7, label='Phoenix')
    ax.set_xlabel('Frequency (Hz)')
    ax.set_ylabel('Magnitude')
    ax.set_title('Power Spectrum')
    ax.set_xlim([0, 4000])
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # 4. Symbol constellation
    ax = axes[1, 1]
    ax.scatter(np.real(brain_symbols[:200]), np.imag(brain_symbols[:200]), 
               c='blue', alpha=0.5, s=20, label='Brain')
    ax.scatter(np.real(phoenix_symbols[:200]), np.imag(phoenix_symbols[:200]), 
               c='red', alpha=0.5, s=20, label='Phoenix')
    ax.set_xlabel('I')
    ax.set_ylabel('Q')
    ax.set_title('Symbol Constellation (first 200)')
    ax.axis('equal')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # 5. Phase vs time
    ax = axes[2, 0]
    brain_phase = np.unwrap(np.angle(brain_symbols))
    phoenix_phase = np.unwrap(np.angle(phoenix_symbols))
    t_sym = np.arange(min(500, len(brain_phase))) / SYMBOL_RATE * 1000
    ax.plot(t_sym, brain_phase[:len(t_sym)], 'b-', alpha=0.7, label='Brain')
    ax.plot(t_sym, phoenix_phase[:len(t_sym)], 'r-', alpha=0.7, label='Phoenix')
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Phase (rad)')
    ax.set_title('Unwrapped Phase')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # 6. Phase difference
    ax = axes[2, 1]
    t_sym = np.arange(len(phase_diff)) / SYMBOL_RATE * 1000
    ax.plot(t_sym, phase_diff, 'g-', linewidth=0.5)
    ax.axhline(y=0, color='k', linestyle='--', alpha=0.3)
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Phase Diff (rad)')
    ax.set_title('Phase Difference (detrended)')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('tx_comparison.png', dpi=150)
    print("\nPlot saved to: tx_comparison.png")
    plt.show()


def main():
    print("="*60)
    print("TX OUTPUT COMPARISON: Brain vs Phoenix Nest")
    print("="*60)
    print(f"Test mode: {TEST_MODE}")
    print(f"Test message: {len(TEST_MESSAGE)} bytes")
    
    # Connect to Brain modem
    brain_ctrl, brain_data = connect_modem("Brain", BRAIN_CONTROL, BRAIN_DATA)
    if brain_ctrl is None:
        print("\nERROR: Could not connect to Brain modem")
        return 1
    
    # Connect to Phoenix Nest modem
    phoenix_ctrl, phoenix_data = connect_modem("Phoenix", PHOENIX_CONTROL, PHOENIX_DATA)
    if phoenix_ctrl is None:
        print("\nERROR: Could not connect to Phoenix Nest modem")
        brain_ctrl.close()
        brain_data.close()
        return 1
    
    try:
        # Generate TX on both
        brain_pcm = generate_tx("Brain", brain_ctrl, brain_data, TEST_MODE, TEST_MESSAGE)
        phoenix_pcm = generate_tx("Phoenix", phoenix_ctrl, phoenix_data, TEST_MODE, TEST_MESSAGE)
        
        if not brain_pcm:
            print("\nERROR: Brain modem did not generate PCM file")
            return 1
        if not phoenix_pcm:
            print("\nERROR: Phoenix modem did not generate PCM file")
            return 1
        
        # Load PCM files
        print("\n" + "="*60)
        print("LOADING PCM FILES")
        print("="*60)
        
        brain_samples = load_pcm_file(brain_pcm)
        phoenix_samples = load_pcm_file(phoenix_pcm)
        
        if brain_samples is None or phoenix_samples is None:
            return 1
        
        # Compare signals
        aligned_brain, aligned_phoenix, lag = compare_signals(brain_samples, phoenix_samples)
        
        # Compare symbols
        brain_symbols, phoenix_symbols, phase_diff = compare_symbols(
            aligned_brain, aligned_phoenix)
        
        # Analyze preambles
        print("\n" + "="*60)
        print("PREAMBLE ANALYSIS")
        print("="*60)
        analyze_preamble(brain_samples, "Brain")
        analyze_preamble(phoenix_samples, "Phoenix")
        
        # Generate plots
        generate_plots(aligned_brain, aligned_phoenix, brain_symbols, phoenix_symbols,
                      phase_diff, lag)
        
        print("\n" + "="*60)
        print("COMPARISON COMPLETE")
        print("="*60)
        
    finally:
        brain_ctrl.close()
        brain_data.close()
        phoenix_ctrl.close()
        phoenix_data.close()
    
    return 0


if __name__ == "__main__":
    exit(main())
