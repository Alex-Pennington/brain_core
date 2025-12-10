#!/usr/bin/env python3
"""
Simple test client for Paul Brain Modem Core server.
Tests basic commands and TX/RX functionality.
"""

import socket
import time
import sys

CONTROL_PORT = 3999
DATA_PORT = 3998
HOST = 'localhost'

def send_command(sock, cmd):
    """Send command and get response"""
    sock.sendall((cmd + '\n').encode())
    time.sleep(0.1)
    try:
        response = sock.recv(4096).decode().strip()
        return response
    except:
        return None

def main():
    print("Paul Brain Modem Core - Test Client")
    print("=" * 40)
    
    # Connect to control port
    print(f"\nConnecting to control port {CONTROL_PORT}...")
    ctrl = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        ctrl.connect((HOST, CONTROL_PORT))
        ctrl.settimeout(2.0)
    except ConnectionRefusedError:
        print("ERROR: Server not running. Start brain_modem_server first.")
        return 1
    
    # Get welcome message
    welcome = ctrl.recv(1024).decode().strip()
    print(f"Server: {welcome}")
    
    # Test commands
    tests = [
        ("CMD:QUERY:VERSION", "Get version"),
        ("CMD:QUERY:MODES", "List modes"),
        ("CMD:DATA RATE:600S", "Set mode to 600S"),
        ("CMD:QUERY:STATUS", "Get status"),
    ]
    
    print("\n--- Command Tests ---")
    for cmd, desc in tests:
        print(f"\n{desc}: {cmd}")
        response = send_command(ctrl, cmd)
        print(f"  Response: {response}")
    
    # Connect to data port
    print(f"\n--- Data Port Test ---")
    print(f"Connecting to data port {DATA_PORT}...")
    data = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    data.connect((HOST, DATA_PORT))
    data.settimeout(2.0)
    
    # Send test data
    test_data = b"Hello, Paul Brain Modem!"
    print(f"Sending {len(test_data)} bytes to data port...")
    data.sendall(test_data)
    time.sleep(0.1)
    
    # Trigger transmit
    print("Triggering TX with CMD:SENDBUFFER...")
    response = send_command(ctrl, "CMD:SENDBUFFER")
    print(f"  Response: {response}")
    
    # Check for PCM file
    response = send_command(ctrl, "CMD:QUERY:PCM OUTPUT")
    print(f"  PCM Output: {response}")
    
    print("\n--- Test Complete ---")
    print("Check tx_pcm_out/ directory for generated PCM file.")
    
    ctrl.close()
    data.close()
    return 0

if __name__ == "__main__":
    sys.exit(main())
