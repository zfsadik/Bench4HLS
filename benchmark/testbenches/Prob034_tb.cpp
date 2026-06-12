#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 150;  // Total number of rising-edge events to simulate

    // Reference model for the expected behavior of the decade counter.
    ap_uint<4> ref_state = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ap_uint<4> dut_q = 0;

    // --- Fixed Sequence for Reset Behavior ---
    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(true, dut_q);
        ref_state = 1;
        cycleCount++;

        if (dut_q != ref_state) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": reset active, expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // Now, run a fixed sequence to check counting from 1 to 10.
    for (int i = 0; i < 12; i++) {
        if (ref_state == 10) {
            ref_state = 1;
        } else {
            ref_state = ref_state + 1;
        }
        TopModule(false, dut_q);
        cycleCount++;

        if (dut_q != ref_state) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // --- Random Test Sequence ---
    for (int i = 0; i < totalCycles; i++) {
        // Randomly generate reset (0 or 1).
        bool reset = (std::rand() % 2) ? true : false;
        // For the reference model: if reset is high, counter resets to 1; else increment.
        if (reset || ref_state == 10) {
            ref_state = 1;
        } else {
            ref_state = ref_state + 1;
        }
        TopModule(reset, dut_q);
        cycleCount++;

        if (dut_q != ref_state) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": reset = " << reset
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
