#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<4> in, ap_uint<2> &pos);

void ref_compute(ap_uint<4> in, ap_uint<2> &pos_ref) {
    if      (in[0]) pos_ref = 0;
    else if (in[1]) pos_ref = 1;
    else if (in[2]) pos_ref = 2;
    else if (in[3]) pos_ref = 3;
    else            pos_ref = 0;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;
    
    ap_uint<4> in;
    ap_uint<2> pos_dut;
    ap_uint<2> pos_ref;
    
    // --- Phase 1: Fixed Test Cases ---
    // Test all 16 combinations.
    for (unsigned int val = 0; val < 16; val++) {
        in = val;
        TopModule(in, pos_dut);
        ref_compute(in, pos_ref);
        if (pos_dut != pos_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << pos_ref 
                      << ", got " << pos_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Cases ---
    // Run 50 random test cases.
    for (int i = 0; i < 50; i++) {
        in = std::rand() & 0xF;  // Random 4-bit value.
        TopModule(in, pos_dut);
        ref_compute(in, pos_ref);
        if (pos_dut != pos_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << pos_ref 
                      << ", got " << pos_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount 
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount 
                  << " cases." << std::endl;
    }
    
    return 0;
}
