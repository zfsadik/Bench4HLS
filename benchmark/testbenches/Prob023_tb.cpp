#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<100> in, ap_uint<100> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int cycle = 0; cycle < totalCycles; cycle++) {
        ap_uint<100> in_val = 0;
        for (int i = 0; i < 4; i++) {
            ap_uint<100> part = (((ap_uint<100>)std::rand() << 15) | std::rand()) & 0x1FFFFFF;
            in_val |= part << (25 * i);
        }

        ap_uint<100> dutOut;
        TopModule(in_val, dutOut);

        ap_uint<100> expected = 0;
        for (int i = 0; i < 100; i++) {
            expected[i] = in_val[100 - i - 1];
        }

        // Compare DUT output with expected output.
        if (dutOut != expected) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": in = " << in_val 
                      << ", expected = " << expected 
                      << ", got = " << dutOut << std::endl;
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
