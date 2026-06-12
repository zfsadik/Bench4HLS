#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> in, bool &parity);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int cycle = 0; cycle < totalCycles; cycle++) {
        // Generate a random 8-bit input.
        ap_uint<8> in_val = std::rand() % 256;
        
        bool dutParity = false;
        TopModule(in_val, dutParity);

        bool expected = false;
        for (int i = 0; i < 8; i++) {
            expected ^= in_val[i];
        }
        
        // Compare the DUT output with the expected output.
        if (dutParity != expected) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": in = " << in_val 
                      << ", expected parity = " << expected 
                      << ", got parity = " << dutParity << std::endl;
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
