#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1> in, ap_uint<1> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total test cycles

    // Reference model for expected behavior:
    ap_uint<1> ref_state = 0;
    ap_uint<1> dut_q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulate test cycles:
    for (int i = 0; i < totalCycles; i++) {
        // Generate a random input for 'in' (0 or 1).
        ap_uint<1> in_val = std::rand() % 2;

        TopModule(in_val, dut_q);

        ref_state = in_val ^ ref_state;

        // Compare DUT output with expected value.
        if (dut_q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": in = " << in_val
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
