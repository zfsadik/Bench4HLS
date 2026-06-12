#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> d, bool reset, ap_uint<8> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 150;  // Total test cycles (rising edges)

    // Reference model for expected behavior of the D flip-flops.
    ap_uint<8> ref_state = 0;
    ap_uint<8> dut_q = 0;

    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulation loop: Each cycle simulates a rising edge.
    for (int i = 0; i < totalCycles; i++) {
        ap_uint<8> d;
        bool reset;

        // Apply reset for first 2 cycles, then random stimulus.
        if (i < 2) {
            d = 0;
            reset = true;
        } else {
            // Generate a random 8-bit data input.
            d = std::rand() % 256;
            // Generate a random reset signal (0 or 1).
            reset = (std::rand() % 2) ? true : false;
        }

        // Simulate rising edge: one call = one rising edge.
        TopModule(d, reset, dut_q);

        // Update reference model: on rising edge, if reset is high, expected becomes 0; else expected becomes d.
        if (reset)
            ref_state = 0;
        else
            ref_state = d;

        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Error at cycle " << cycleCount
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

    // Always return 0.
    return 0;
}
