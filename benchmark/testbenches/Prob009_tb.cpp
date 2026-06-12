#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<3> in, ap_uint<2>& out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // --- Fixed Test Sequence ---
    // Test all possible 3-bit values (0 to 7).
    for (int val = 0; val < 8; val++) {
        ap_uint<3> in = val;
        ap_uint<2> out;
        TopModule(in, out);

        // Calculate expected population count.
        unsigned int expected = 0;
        for (int bit = 0; bit < 3; bit++) {
            expected += ((in >> bit) & 1);
        }
        if (out != expected) {
            std::cout << "Fixed test error for input " << in 
                      << ": expected " << expected 
                      << ", got " << out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    // Seed the random number generator.
    std::srand(42);
    const int randomTests = 200;
    for (int i = 0; i < randomTests; i++) {
        // Generate a random 3-bit value (0 to 7).
        ap_uint<3> in = std::rand() % 8;
        ap_uint<2> out;
        TopModule(in, out);

        // Compute the expected population count.
        unsigned int expected = 0;
        for (int bit = 0; bit < 3; bit++) {
            expected += ((in >> bit) & 1);
        }
        if (out != expected) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << " for input " << in 
                      << ": expected " << expected 
                      << ", got " << out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report summary of test results.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    return 0;
}
