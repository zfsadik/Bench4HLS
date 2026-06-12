#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCycles = 20;   // Number of fixed-cycle tests.
    const int randomCycles = 400; // Number of random test cycles.

    // Reference model for the expected counter value.
    ap_uint<4> ref_state = 0;
    ap_uint<4> dut_q = 0;

    // --- Fixed Test Sequence: Verify synchronous reset and counting behavior ---
    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(true, dut_q);
        ref_state = 0;
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (reset) at cycle " << cycleCount
                      << ": expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // Test counting without reset.
    for (int i = 0; i < fixedCycles; i++) {
        TopModule(false, dut_q);

        if (ref_state == 15) {
            ref_state = 0;
        } else {
            ref_state = ref_state + 1;
        }
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (count) at cycle " << cycleCount
                      << ": expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // --- Random Test Sequence ---
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < randomCycles; i++) {
        // Generate a random reset signal.
        bool reset = (std::rand() % 2) ? true : false;
        TopModule(reset, dut_q);
        // Update the reference model:
        if (reset) {
            ref_state = 0;
        } else if (ref_state == 15) {
            ref_state = 0;
        } else {
            ref_state = ref_state + 1;
        }
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
