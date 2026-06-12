#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<3> a, ap_uint<16> & q);

ap_uint<16> ref_model(ap_uint<3> a) {
    const ap_uint<16> lut[8] = {
        0x1232, 0xAEE0, 0x27D4, 0x5A0E,
        0x2066, 0x64CE, 0xC526, 0x2F19
    };
    return lut[a];
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    ap_uint<3> a;
    ap_uint<16> q_dut;  // Output from the design under test.
    ap_uint<16> q_ref;  // Expected output from the reference model.

    // --- Phase 1: Sequential Test on 8 Cases ---
    for (int count = 0; count < 8; count++) {
        a = count;
        // Apply the input.
        TopModule(a, q_dut);
        // Compute the reference output.
        q_ref = ref_model(a);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Testing for 200 Cases ---
    for (int i = 0; i < 200; i++) {
        // Generate a random 3-bit value.
        a = rand() & 0x7;
        TopModule(a, q_dut);
        q_ref = ref_model(a);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Test Summary ---
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
