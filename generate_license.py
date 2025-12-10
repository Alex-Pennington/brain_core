#!/usr/bin/env python3
"""
Generate a valid license key for the current hardware.
Usage: python3 generate_license.py [customer_id] [expiry_date]
"""

import sys
import subprocess
import datetime
import tempfile
import os
import shutil

def get_hardware_id():
    """Get hardware ID by compiling and running a small C++ program"""
    cpp_code = '''
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

int main() {
    uint32_t cpu_info[4] = {0};
#ifdef _WIN32
    __cpuid((int*)cpu_info, 1);
#else
    __get_cpuid(1, &cpu_info[0], &cpu_info[1], &cpu_info[2], &cpu_info[3]);
#endif
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << cpu_info[0];
    ss << std::setw(8) << cpu_info[3];
    std::cout << ss.str();
    return 0;
}
'''
    
    # Check if g++ is available
    if shutil.which('g++') is None:
        print("ERROR: g++ compiler not found. Please install g++.")
        sys.exit(1)
    
    # Use temporary directory for cross-platform compatibility
    with tempfile.TemporaryDirectory() as tmpdir:
        cpp_file = os.path.join(tmpdir, 'get_hwid.cpp')
        exe_file = os.path.join(tmpdir, 'get_hwid')
        
        # Write temp file
        with open(cpp_file, 'w') as f:
            f.write(cpp_code)
        
        # Compile
        try:
            subprocess.run(['g++', '-o', exe_file, cpp_file], 
                         check=True, capture_output=True, text=True)
        except subprocess.CalledProcessError as e:
            print(f"ERROR: Compilation failed: {e.stderr}")
            sys.exit(1)
        
        # Run
        try:
            result = subprocess.run([exe_file], capture_output=True, text=True, check=True)
        except subprocess.CalledProcessError as e:
            print(f"ERROR: Execution failed: {e.stderr}")
            sys.exit(1)
        
        return result.stdout.strip()

def compute_checksum(data):
    """Compute license checksum matching the C++ implementation"""
    hash_val = 0x5A5A5A5A  # Seed
    
    for c in data:
        hash_val ^= ord(c)
        hash_val = ((hash_val << 7) | (hash_val >> 25)) & 0xFFFFFFFF  # Rotate left 7 bits
        hash_val ^= 0x12345678
    
    return f"{hash_val:08X}"

def main():
    customer = sys.argv[1] if len(sys.argv) > 1 else "TEST01"
    
    if len(sys.argv) > 2:
        expiry = sys.argv[2]
    else:
        # Default: 1 year from now
        next_year = datetime.datetime.now() + datetime.timedelta(days=365)
        expiry = next_year.strftime("%Y%m%d")
    
    print("Generating license key...")
    print(f"Customer ID: {customer}")
    print(f"Expiry Date: {expiry}")
    
    hw_id = get_hardware_id()
    print(f"Hardware ID: {hw_id}")
    
    data = customer + hw_id + expiry
    checksum = compute_checksum(data)
    
    license_key = f"{customer}-{hw_id}-{expiry}-{checksum}"
    
    print(f"\nLicense Key: {license_key}")
    print("\nWriting to license.key...")
    
    with open('license.key', 'w') as f:
        f.write(license_key + '\n')
    
    print("Done! License key written to license.key")

if __name__ == '__main__':
    main()
