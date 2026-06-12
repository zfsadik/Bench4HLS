#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<10> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCycles = 20;   // Number of fixed-cycle tests
    const int randomCycles = 1000; // Number of random test cycles

    // Reference model for the counter.
    ap_uint<10> ref_state = 0;

    ap_uint<10> dut_q = 0;

    // --- Fixed Test Sequence: Synchronous reset and count-up behavior ---
    // First, force a reset.
    for (int i = 0; i < 2; i++) {
        bool reset = true;
        TopModule(reset, dut_q);
        ref_state = 0; // Reference: with reset, counter becomes 0.
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (reset) at cycle " << cycleCount
                      << ": expected q = " << ref_state << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // Now, test counting without reset.
    for (int i = 0; i < fixedCycles; i++) {
        bool reset = false;
        TopModule(reset, dut_q);
        // Update reference model: if ref_state == 999 then wrap around to 0, else increment.
        if (ref_state == 999)
            ref_state = 0;
        else
            ref_state = ref_state + 1;
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (count) at cycle " << cycleCount
                      << ": expected q = " << ref_state << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // --- Random Test Sequence ---
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < randomCycles; i++) {
        // Generate a random reset signal.
        bool reset = (std::rand() % 2) ? true : false;

        // Update reference model based on reset.
        if (reset || (ref_state == 999))
            ref_state = 0;
        else
            ref_state = ref_state + 1;

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
