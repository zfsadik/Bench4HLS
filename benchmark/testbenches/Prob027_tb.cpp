#include <iostream>
#include <cstdlib>

void TopModule(bool d, bool ena, bool &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Number of test cycles

    bool ref_state = 0;  // Initial state is 0.

    // Seed the random number generator with a fixed seed for reproducibility.
    std::srand(42);

    // Verify static state initializes to 0.
    {
        bool q_init = true;
        TopModule(false, false, q_init);
        if (q_init != false) {
            std::cout << "Mismatch: initial state expected 0, got " << q_init << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    for (int i = 0; i < totalCycles; i++) {
        // Generate random boolean values for d and ena.
        bool d = (std::rand() % 2) ? true : false;
        bool ena = (std::rand() % 2) ? true : false;

        if (ena) {
            ref_state = d;
        }
        bool expected = ref_state;

        bool dut_q = false;
        TopModule(d, ena, dut_q);

        // Compare the DUT output with the expected value.
        if (dut_q != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": d = " << d
                      << ", ena = " << ena
                      << ", expected q = " << expected
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
