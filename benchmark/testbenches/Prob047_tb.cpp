#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool d, bool sr, bool &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total number of test cycles

    // Reference model for expected behavior:
    bool ref_state = 0;
    bool dut_q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(false, true, dut_q);
        ref_state = 0;
    }

    // Simulation loop:
    for (int i = 0; i < totalCycles; i++) {
        // Generate random inputs for d and sr.
        bool d = (std::rand() % 2) ? true : false;
        bool sr = (std::rand() % 2) ? true : false;

        TopModule(d, sr, dut_q);

        if (sr)
            ref_state = 0;
        else
            ref_state = d;

        // Check DUT output against expected value.
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": d = " << d
                      << ", sr = " << sr
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
