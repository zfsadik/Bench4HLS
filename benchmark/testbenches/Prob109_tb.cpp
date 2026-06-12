#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<1> &f);

void ref_compute(ap_uint<4> x, ap_uint<1> &f_ref) {
    // Hard-coded K-map truth table (x=0..15)
    const ap_uint<1> lut[16] = {1,0,1,0, 0,0,0,0, 1,1,1,0, 1,1,0,1};
    f_ref = lut[x.to_uint()];
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<4> x;
    ap_uint<1> f_dut = 0;
    ap_uint<1> f_ref = 0;

    // --- Phase 1: Fixed Test Cases ---
    // Test all 16 combinations.
    for (unsigned int in_val = 0; in_val < 16; in_val++) {
        x = in_val;  // 4-bit value
        TopModule(x, f_dut);
        ref_compute(x, f_ref);
        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << f_ref
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Cases ---
    // Run 50 random test cases.
    for (int i = 0; i < 50; i++) {
        x = std::rand() & 0xF;  // Random 4-bit number.
        TopModule(x, f_dut);
        ref_compute(x, f_ref);
        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << f_ref
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
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
