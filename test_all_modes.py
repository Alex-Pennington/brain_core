#!/usr/bin/env python3
"""Test all modem modes with loopback"""

import socket
import time
import sys

HOST = '127.0.0.1'
CONTROL_PORT = 3999
DATA_PORT = 3998

# All supported modes
MODES = [
    "75NS", "75NL",
    "150S", "150L", 
    "300S", "300L",
    "600S", "600L",
    "1200S", "1200L",
    "2400S", "2400L",
    "4800S"
]

def send_cmd(sock, cmd, timeout=2.0):
    sock.sendall((cmd + '\n').encode())
    time.sleep(0.1)
    sock.settimeout(timeout)
    try:
        return sock.recv(4096).decode().strip()
    except socket.timeout:
        return None

def drain_responses(sock, timeout=0.5):
    """Drain all pending responses"""
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

def test_mode(mode, ctrl, data):
    """Test a single mode, returns (success, rx_bytes, tx_bytes)"""
    print(f"\n{'='*50}")
    print(f"Testing mode: {mode}")
    print('='*50)
    
    # Clear any pending data
    drain_responses(ctrl)
    try:
        data.settimeout(0.1)
        data.recv(4096)
    except:
        pass
    
    # Set mode
    resp = send_cmd(ctrl, f"CMD:DATA RATE:{mode}")
    if not resp or "OK" not in resp:
        print(f"  [FAIL] Could not set mode: {resp}")
        return False, 0, 0
    print(f"  Mode set: {resp}")
    
    # Send test data - size varies by mode speed
    # Faster modes need more data to fill interleaver
    if "4800" in mode:
        test_data = b"X" * 500
    elif "2400" in mode:
        test_data = b"X" * 400
    elif "1200" in mode:
        test_data = b"X" * 300
    else:
        test_data = b"X" * 200
    
    data.settimeout(5.0)
    data.sendall(test_data)
    print(f"  Sent {len(test_data)} bytes")
    time.sleep(0.2)
    
    # Transmit
    ctrl.sendall(b"CMD:SENDBUFFER\n")
    
    # Wait for TX to complete - slower modes take longer
    if "75" in mode:
        wait_time = 30  # 75 bps is very slow
    elif "150" in mode:
        wait_time = 20
    elif "300" in mode:
        wait_time = 12
    elif "600" in mode:
        wait_time = 8
    elif "1200" in mode:
        wait_time = 6
    elif "2400" in mode:
        wait_time = 5
    else:
        wait_time = 4
    
    print(f"  Waiting {wait_time}s for TX...")
    time.sleep(wait_time)
    
    # Get TX responses
    tx_responses = drain_responses(ctrl, timeout=2.0)
    pcm_file = None
    for r in tx_responses:
        if r.startswith("TX:PCM:"):
            pcm_file = r.split("TX:PCM:")[1]
            print(f"  PCM file: {pcm_file}")
    
    if not pcm_file:
        print(f"  [FAIL] No PCM file generated")
        print(f"  Responses: {tx_responses}")
        return False, 0, len(test_data)
    
    # Inject PCM back for RX
    print(f"  Injecting PCM for RX...")
    ctrl.sendall(f"CMD:RXAUDIOINJECT:{pcm_file}\n".encode())
    
    # Wait for RX - same timing as TX
    time.sleep(wait_time + 2)
    
    # Get RX responses
    rx_responses = drain_responses(ctrl, timeout=2.0)
    rx_complete = False
    detected_mode = None
    for r in rx_responses:
        if "RX:COMPLETE" in r:
            rx_complete = True
        if "RX:MODE:" in r:
            detected_mode = r.split("RX:MODE:")[1]
    
    # Check for received data
    rx_bytes = b""
    data.settimeout(1.0)
    try:
        while True:
            chunk = data.recv(4096)
            if not chunk:
                break
            rx_bytes += chunk
    except socket.timeout:
        pass
    
    success = len(rx_bytes) > 0
    status = "[PASS]" if success else "[FAIL]"
    print(f"  {status} TX:{len(test_data)} bytes, RX:{len(rx_bytes)} bytes")
    if detected_mode:
        print(f"  Detected mode: {detected_mode}")
    
    return success, len(rx_bytes), len(test_data)

def main():
    print("="*60)
    print("Paul Brain Modem Core - All Modes Loopback Test")
    print("="*60)
    
    # Connect
    print("\nConnecting to server...")
    ctrl = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ctrl.settimeout(5.0)
    try:
        ctrl.connect((HOST, CONTROL_PORT))
    except ConnectionRefusedError:
        print("ERROR: Server not running. Start with: .\\brain_modem_server.exe")
        return 1
    
    welcome = ctrl.recv(1024).decode().strip()
    print(f"Server: {welcome}")
    
    data = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    data.settimeout(5.0)
    data.connect((HOST, DATA_PORT))
    print("Connected to data port")
    
    # Test each mode
    results = {}
    for mode in MODES:
        try:
            success, rx, tx = test_mode(mode, ctrl, data)
            results[mode] = {"success": success, "rx": rx, "tx": tx}
        except Exception as e:
            print(f"  [ERROR] {e}")
            results[mode] = {"success": False, "rx": 0, "tx": 0, "error": str(e)}
    
    # Summary
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"{'Mode':<10} {'Status':<10} {'TX bytes':<12} {'RX bytes':<12}")
    print("-"*50)
    
    passed = 0
    failed = 0
    for mode in MODES:
        r = results[mode]
        status = "PASS" if r["success"] else "FAIL"
        if r["success"]:
            passed += 1
        else:
            failed += 1
        print(f"{mode:<10} {status:<10} {r['tx']:<12} {r['rx']:<12}")
    
    print("-"*50)
    print(f"Total: {passed} passed, {failed} failed out of {len(MODES)} modes")
    print("="*60)
    
    ctrl.close()
    data.close()
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
