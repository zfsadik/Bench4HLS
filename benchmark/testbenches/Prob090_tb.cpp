#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool c, bool d, ap_uint<4> &mux_in);

// Reference function: K-map truth table indexed by {c,d} = c*2+d.
// mux_in[0]=ab00 col, mux_in[1]=ab01 col, mux_in[2]=ab11 col, mux_in[3]=ab10 col.
void ref_compute(bool c, bool d, ap_uint<4> &mux_in) {
    // cd: 00->8, 01->1, 10->9, 11->13  (derived directly from K-map columns)
    const ap_uint<4> lut[4] = {8, 1, 9, 13};
    mux_in = lut[(c ? 2 : 0) + (d ? 1 : 0)];
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool c, d;
    ap_uint<4> mux_in_dut;
    ap_uint<4> mux_in_ref;

    // --- Phase 1: Fixed Test Cases ---
    // Test all four possible combinations of {c, d}.
    for (int i = 0; i < 4; i++) {
        c = (i >> 1) & 1; // MSB of the 2-bit number
        d = (i >> 0) & 1; // LSB

        // Call DUT.
        TopModule(c, d, mux_in_dut);
        // Compute reference output.
        ref_compute(c, d, mux_in_ref);

        if (mux_in_dut != mux_in_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << mux_in_ref 
                      << ", got " << mux_in_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Cases ---
    // Repeat 50 times.
    for (int i = 0; i < 50; i++) {
        // Generate random values for c and d.
        c = (std::rand() & 1) ? true : false;
        d = (std::rand() & 1) ? true : false;

        TopModule(c, d, mux_in_dut);
        ref_compute(c, d, mux_in_ref);

        if (mux_in_dut != mux_in_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << mux_in_ref 
                      << ", got " << mux_in_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print the test summary.
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
