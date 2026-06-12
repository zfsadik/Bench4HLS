#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> d, bool reset, ap_uint<8> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 150;  // Number of test cycles

    // Reference model: simulate expected DFF behavior.
    ap_uint<8> ref_state = 0;
    ap_uint<8> dut_q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(0, true, dut_q);
        ref_state = 0;
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": reset = 1, d = 0"
                      << ", expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // Simulation loop: one call per cycle = one rising edge.
    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 8-bit data input.
        ap_uint<8> d = std::rand() % 256;
        // Generate a random synchronous reset signal (true/false).
        bool reset = (std::rand() % 2) ? true : false;

        TopModule(d, reset, dut_q);

        if (reset) {
            ref_state = 0;
        } else {
            ref_state = d;
        }
        cycleCount++;

        // Check DUT output against expected value.
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": reset = " << reset
                      << ", d = " << d
                      << ", expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
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
