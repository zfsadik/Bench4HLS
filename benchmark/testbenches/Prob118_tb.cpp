#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, bool &f);

void ref_compute(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, bool &f_ref) {
    f_ref = (bool)(a ^ b ^ c ^ d);
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT signals.
    ap_uint<1> a, b, c, d;
    bool f_dut = false;

    // Reference output.
    bool f_ref = false;

    // Run fixed test cases for all 16 combinations.
    for (int i = 0; i < 16; i++) {
        // Set a 4-bit value in a, b, c, d using bit extraction.
        ap_uint<4> value = i;
        a = value[3];  // MSB
        b = value[2];
        c = value[1];
        d = value[0];

        TopModule(a, b, c, d, f_dut);
        ref_compute(a, b, c, d, f_ref);

        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << f_ref 
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Run additional random test cases.
    const int numRandom = 50;
    for (int i = 0; i < numRandom; i++) {
        // Generate random bits.
        a = (std::rand() & 1);
        b = (std::rand() & 1);
        c = (std::rand() & 1);
        d = (std::rand() & 1);

        TopModule(a, b, c, d, f_dut);
        ref_compute(a, b, c, d, f_ref);

        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << f_ref 
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount 
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount 
                  << " cases." << std::endl;
    }

    return 0;
}
