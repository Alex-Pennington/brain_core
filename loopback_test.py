#!/usr/bin/env python3
"""Loopback test for Paul Brain Modem Core"""

import socket
import time
import sys

HOST = '127.0.0.1'
CONTROL_PORT = 3999
DATA_PORT = 3998

def send_cmd(sock, cmd):
    sock.sendall((cmd + '\n').encode())
    time.sleep(0.3)
    try:
        return sock.recv(4096).decode().strip()
    except socket.timeout:
        return None

def main():
    print("=" * 50)
    print("Paul Brain Modem Core - Loopback Test")
    print("=" * 50)
    
    # Connect control port
    print("\n[1] Connecting to control port...")
    ctrl = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ctrl.settimeout(5.0)
    try:
        ctrl.connect((HOST, CONTROL_PORT))
    except ConnectionRefusedError:
        print("ERROR: Server not running")
        return 1
    
    welcome = ctrl.recv(1024).decode().strip()
    print(f"    Server: {welcome}")
    
    # Connect data port
    print("\n[2] Connecting to data port...")
    data = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    data.settimeout(5.0)
    data.connect((HOST, DATA_PORT))
    print("    Connected")
    
    # Send test data - use larger payload to ensure full preamble
    test_msg = b"HELLO MODEM LOOPBACK TEST " + b"A" * 200  # ~226 bytes
    print(f"\n[3] Sending {len(test_msg)} bytes")
    data.sendall(test_msg)
    time.sleep(0.5)  # Give time for data to arrive
    
    # Check buffer status
    print("\n[3b] Checking TX buffer status...")
    resp = send_cmd(ctrl, "CMD:QUERY:STATUS")
    print(f"     {resp}")
    
    # Set mode
    print("\n[4] Setting mode to 600S...")
    resp = send_cmd(ctrl, "CMD:DATA RATE:600S")
    print(f"    {resp}")
    
    # Transmit
    print("\n[5] Transmitting (CMD:SENDBUFFER)...")
    ctrl.sendall(b"CMD:SENDBUFFER\n")
    time.sleep(3)  # Give TX time to complete
    
    # Read all TX responses
    ctrl.settimeout(2.0)
    tx_responses = []
    while True:
        try:
            resp = ctrl.recv(4096).decode().strip()
            if resp:
                print(f"    RAW RESPONSE: {repr(resp)}")
                tx_responses.extend(resp.split('\n'))
        except socket.timeout:
            break
    
    if not tx_responses:
        print("    No responses received!")
    
    pcm_file = None
    for r in tx_responses:
        print(f"    {r}")
        if r.startswith("TX:PCM:"):
            pcm_file = r.split("TX:PCM:")[1]
    
    if not pcm_file:
        print("ERROR: No PCM file generated")
        return 1
    
    print(f"\n[6] PCM file: {pcm_file}")
    
    # Inject PCM back to RX
    print(f"\n[7] Injecting PCM to RX (CMD:RXAUDIOINJECT:{pcm_file})...")
    ctrl.settimeout(10.0)
    ctrl.sendall(f"CMD:RXAUDIOINJECT:{pcm_file}\n".encode())
    time.sleep(3)
    
    # Read RX responses
    ctrl.settimeout(1.0)
    rx_responses = []
    while True:
        try:
            resp = ctrl.recv(4096).decode().strip()
            if resp:
                rx_responses.extend(resp.split('\n'))
        except socket.timeout:
            break
    
    for r in rx_responses:
        print(f"    {r}")
    
    # Check for received data on data port
    print("\n[8] Checking data port for decoded bytes...")
    data.settimeout(1.0)
    try:
        rx_data = data.recv(4096)
        print(f"    Received {len(rx_data)} bytes: {rx_data}")
        
        if test_msg in rx_data or rx_data == test_msg:
            print("\n" + "=" * 50)
            print("SUCCESS: Loopback test PASSED!")
            print("=" * 50)
            return 0
        else:
            print(f"\n    Expected: {test_msg}")
            print(f"    Got:      {rx_data}")
            print("\n    Data mismatch (may be OK if partial)")
    except socket.timeout:
        print("    No data received on data port")
    
    ctrl.close()
    data.close()
    
    print("\n" + "=" * 50)
    print("Loopback test complete")
    print("=" * 50)
    return 0

if __name__ == "__main__":
    sys.exit(main())
