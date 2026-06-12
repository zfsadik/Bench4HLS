#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool d, bool r, bool &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100; // Total test cycles

    // Reference model for expected behavior.
    bool ref_state = 0;
    bool dut_q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulation loop: one call per cycle = one rising edge.
    for (int i = 0; i < totalCycles; i++) {
        // Generate random inputs.
        bool d = (std::rand() % 2) ? true : false;
        bool r = (std::rand() % 2) ? true : false;

        TopModule(d, r, dut_q);

        if (r)
            ref_state = 0;
        else
            ref_state = d;

        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": d = " << d
                      << ", r = " << r
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
