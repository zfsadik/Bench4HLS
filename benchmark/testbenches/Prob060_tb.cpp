#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool shift_ena, bool count_ena, bool data, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 200; // Total test cycles

    // Reference model for expected behavior.
    ap_uint<4> ref_state = 0;
    ap_uint<4> dut_q = 0;

    // Seed random number generator.
    std::srand(42);

    // Test loop:
    for (int i = 0; i < totalCycles; i++) {
        bool shift_ena = (std::rand() % 3 == 0); // Approximately 1/3 chance to be true
        bool count_ena = (std::rand() % 3 == 1);   // Approximately 1/3 chance to be true
        // Ensure both are not true simultaneously:
        if (shift_ena && count_ena) count_ena = false;

        bool data = (std::rand() % 2) ? true : false;

        TopModule(shift_ena, count_ena, data, dut_q);

        // Update reference model on rising edge:
        if (shift_ena) {
            ref_state = (ref_state << 1) | data;
        }
        else if (count_ena) {
            ref_state = ref_state - 1;
        }

        cycleCount++;

        // Compare DUT output with expected reference model.
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": shift_ena = " << shift_ena
                      << ", count_ena = " << count_ena
                      << ", data = " << data
                      << ", expected q = " << (unsigned int)ref_state
                      << ", got q = " << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
