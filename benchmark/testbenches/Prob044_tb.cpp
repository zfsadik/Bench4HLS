#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> d, bool reset, ap_uint<8> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 150;  // Total test cycles

    // Reference model to simulate expected DFF behavior.
    ap_uint<8> ref_state = 0x34;
    ap_uint<8> dut_q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(0, true, dut_q);
        ref_state = 0x34;
        cycleCount++;
    }

    // Deterministic non-reset sequence: 5 cycles with reset=0 and known d pattern.
    ap_uint<8> det_d_vals[5] = {0x00, 0xFF, 0xAA, 0x55, 0x34};
    for (int i = 0; i < 5; i++) {
        TopModule(det_d_vals[i], false, dut_q);
        ref_state = det_d_vals[i];

        if (dut_q != ref_state) {
            std::cout << "Error at cycle " << cycleCount
                      << ": reset = 0"
                      << ", d = " << det_d_vals[i]
                      << ", expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Simulation loop: one call per cycle.
    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 8-bit value for d.
        ap_uint<8> d = std::rand() % 256;
        // Generate a random reset signal (0 or 1).
        bool reset = (std::rand() % 2) ? true : false;

        TopModule(d, reset, dut_q);

        if (reset)
            ref_state = 0x34;
        else
            ref_state = d;

        // Compare DUT output with expected value.
        if (dut_q != ref_state) {
            std::cout << "Error at cycle " << cycleCount
                      << ": reset = " << reset
                      << ", d = " << d
                      << ", expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }

        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
