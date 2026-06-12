#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<4> a, ap_uint<4> b, ap_uint<4> c, ap_uint<4> d, ap_uint<4> e, ap_uint<4> & q);

ap_uint<4> ref_model(ap_uint<4> a, ap_uint<4> b, ap_uint<4> c, ap_uint<4> d, ap_uint<4> e) {
    ap_uint<4> result;
    switch(c) {
        case 0: result = b; break;
        case 1: result = e; break;
        case 2: result = a; break;
        case 3: result = d; break;
        default: result = 0xF; break;
    }
    return result;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    ap_uint<4> a, b, c, d, e;
    ap_uint<4> q_dut; // Design under test output.
    ap_uint<4> q_ref; // Expected reference output.

    // --- Phase 1: Sequential Testing (16 cases) ---
    for (unsigned int count = 0; count < 16; count++) {
        // Choose a fixed pattern for a, b, d, e; vary c from 0 to 15. For simplicity, let a, b, d, e be constant values.
        a = 0xA;       // Example value: A
        b = 0xB;       // Example value: B
        c = count & 0xF; // Varying from 0 to 15.
        d = 0xD;       // Example value: D
        e = 0xE;       // Example value: E

        TopModule(a, b, c, d, e, q_dut);
        q_ref = ref_model(a, b, c, d, e);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << q_ref 
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Testing (100 cases) ---
    for (int i = 0; i < 100; i++) {
        // Generate a random 20-bit value and assign its five 4-bit segments.
        unsigned int rnd = rand() & 0xFFFFF; // 20-bit random number.
        a = (rnd >> 16) & 0xF;
        b = (rnd >> 12) & 0xF;
        c = (rnd >> 8)  & 0xF;
        d = (rnd >> 4)  & 0xF;
        e = rnd & 0xF;

        TopModule(a, b, c, d, e, q_dut);
        q_ref = ref_model(a, b, c, d, e);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << q_ref 
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Final Test Summary ---
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
