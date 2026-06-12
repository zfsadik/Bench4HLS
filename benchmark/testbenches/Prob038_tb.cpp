#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCycles = 20;   // Fixed-cycle tests
    const int randomCycles = 400; // Random test cycles

    // Reference model for expected counter value.
    ap_uint<4> ref_state = 0;
    ap_uint<4> dut_q = 0;

    // --- Fixed Test Sequence ---
    // 1. Force a synchronous reset.
    for (int i = 0; i < 2; i++) {
        TopModule(true, dut_q);
        ref_state = 0;  // Expected output is 0 when reset is high.
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (reset) at cycle " << cycleCount
                      << ": expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // 2. Test counting without reset.
    for (int i = 0; i < fixedCycles; i++) {
        TopModule(false, dut_q);
        // Update reference model.
        if (ref_state == 9)
            ref_state = 0;
        else
            ref_state = ref_state + 1;
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error (count) at cycle " << cycleCount
                      << ": expected q = " << ref_state
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
    }

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < randomCycles; i++) {
        // Generate a random reset signal.
        bool reset = (std::rand() % 2) ? true : false;
        TopModule(reset, dut_q);
        // Update reference model.
        if (reset)
            ref_state = 0;
        else if (ref_state == 9)
            ref_state = 0;
        else
            ref_state = ref_state + 1;
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
