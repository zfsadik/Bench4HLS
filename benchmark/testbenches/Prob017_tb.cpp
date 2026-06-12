#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<100> a, ap_uint<100> b, bool sel, ap_uint<100> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCycles = 6;
    const int randomCycles = 100;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Phase 1: Fixed Test Sequence ---
    ap_uint<100> a_fixed = 0xAABBCC; 
    ap_uint<100> b_fixed = 0xDDEEFF;  

    bool sel = false;
    ap_uint<100> dutOut;
    // For fixed cycles, toggle sel.
    for (int i = 0; i < fixedCycles; i++) {
        sel = !sel; // Toggle the selector.
        TopModule(a_fixed, b_fixed, sel, dutOut);
        // Expected output: a_fixed when sel is 0, b_fixed when sel is 1.
        ap_uint<100> expected = sel ? b_fixed : a_fixed;
        if (dutOut != expected) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": sel = " << sel
                      << ", expected = " << expected 
                      << ", got = " << dutOut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Sequence ---
    for (int i = 0; i < randomCycles; i++) {

        ap_uint<100> a_rand = (ap_uint<100>(std::rand()) << 69) | 
                              (ap_uint<100>(std::rand()) << 38) | 
                              (ap_uint<100>(std::rand()) << 7) | 
                              (std::rand() & 0x7F);
        ap_uint<100> b_rand = (ap_uint<100>(std::rand()) << 69) | 
                              (ap_uint<100>(std::rand()) << 38) | 
                              (ap_uint<100>(std::rand()) << 7) | 
                              (std::rand() & 0x7F);

        // Randomly generate the selector.
        bool sel_rand = (std::rand() % 2) ? true : false;

        TopModule(a_rand, b_rand, sel_rand, dutOut);
        ap_uint<100> expected = sel_rand ? b_rand : a_rand;
        if (dutOut != expected) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": sel = " << sel_rand
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
