#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_int<8> a, ap_int<8> b, ap_int<8> &s, bool &overflow);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 200;  // Number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Fixed Test Cases ---
    {
        const int numFixed = 6;
        ap_int<8> fixedTests[numFixed][2] = {
            {  0,  0 },
            {  70,  10 },
            { -70, -10 },
            {  127,  1 },  // Expected overflow
            { -128, -1 },  // Expected overflow
            {  50, -50 }
        };

        for (int i = 0; i < numFixed; i++) {
            ap_int<8> a = fixedTests[i][0];
            ap_int<8> b = fixedTests[i][1];
            ap_int<8> dut_s = 0;
            bool dut_overflow = false;

            TopModule(a, b, dut_s, dut_overflow);

            // Compute expected sum using 9-bit arithmetic.
            ap_int<9> sum = a + b;
            ap_int<8> expected_s = sum.range(7,0);
            bool expected_overflow = false;
            bool sign_a = (a < 0);
            bool sign_b = (b < 0);
            bool sign_s = (expected_s < 0);
            expected_overflow = (!(sign_a ^ sign_b)) && (sign_a != sign_s);

            if (dut_s != expected_s) {
                std::cout << "Fixed test error (s) at cycle " << cycleCount 
                          << ": a = " << a << ", b = " << b 
                          << ", expected s = " << expected_s 
                          << ", got s = " << dut_s << std::endl;
                errorCount++;
            }
            if (dut_overflow != expected_overflow) {
                std::cout << "Fixed test error (overflow) at cycle " << cycleCount 
                          << ": a = " << a << ", b = " << b 
                          << ", expected overflow = " << expected_overflow 
                          << ", got overflow = " << dut_overflow << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // --- Random Test Cases ---
    for (int i = 0; i < totalCycles; i++) {
        // Generate random 8-bit two's complement numbers.
        ap_int<8> a = static_cast<ap_int<8>>(std::rand() % 256 - 128);
        ap_int<8> b = static_cast<ap_int<8>>(std::rand() % 256 - 128);
        ap_int<8> dut_s = 0;
        bool dut_overflow = false;

        TopModule(a, b, dut_s, dut_overflow);

        // Compute expected results.
        ap_int<9> sum = a + b;
        ap_int<8> expected_s = sum.range(7,0);
        bool sign_a = (a < 0);
        bool sign_b = (b < 0);
        bool sign_s = (expected_s < 0);
        bool expected_overflow = (!(sign_a ^ sign_b)) && (sign_a != sign_s);

        if (dut_s != expected_s) {
            std::cout << "Random test error (s) at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", expected s = " << expected_s 
                      << ", got s = " << dut_s << std::endl;
            errorCount++;
        }
        if (dut_overflow != expected_overflow) {
            std::cout << "Random test error (overflow) at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", expected overflow = " << expected_overflow 
                      << ", got overflow = " << dut_overflow << std::endl;
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
