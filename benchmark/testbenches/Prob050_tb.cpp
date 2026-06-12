#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<100> in, bool &out_and, bool &out_or, bool &out_xor);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCases = 3;   // We'll run a fixed sequence for some edge cases
    const int randomCases = 100; // Plus 100 random test cases

    // --- Fixed Test Sequence ---
    // Test a few fixed patterns:
    // 1. All zeros.
    {
        ap_uint<100> in = 0;
        bool dut_and, dut_or, dut_xor;
        TopModule(in, dut_and, dut_or, dut_xor);
        bool expected_and = false; // not all ones
        bool expected_or  = false; // no bit set
        bool expected_xor = false; // XOR reduction of 0's is 0
        if (dut_and != expected_and) {
            std::cout << "Fixed test error: in = 0, expected out_and = " 
                      << expected_and << ", got = " << dut_and << std::endl;
            errorCount++;
        }
        if (dut_or != expected_or) {
            std::cout << "Fixed test error: in = 0, expected out_or = " 
                      << expected_or << ", got = " << dut_or << std::endl;
            errorCount++;
        }
        if (dut_xor != expected_xor) {
            std::cout << "Fixed test error: in = 0, expected out_xor = " 
                      << expected_xor << ", got = " << dut_xor << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    // 2. All ones.
    {
        ap_uint<100> in = ~((ap_uint<100>)0); // all 100 bits set to 1
        bool dut_and, dut_or, dut_xor;
        TopModule(in, dut_and, dut_or, dut_xor);
        bool expected_and = true;  // all ones => AND is true
        bool expected_or  = true;  // at least one one => OR is true
        // XOR reduction of an even number of 1's is 0.
        // 100 is even so expected_xor = false.
        bool expected_xor = false;
        if (dut_and != expected_and) {
            std::cout << "Fixed test error: in = all ones, expected out_and = " 
                      << expected_and << ", got = " << dut_and << std::endl;
            errorCount++;
        }
        if (dut_or != expected_or) {
            std::cout << "Fixed test error: in = all ones, expected out_or = " 
                      << expected_or << ", got = " << dut_or << std::endl;
            errorCount++;
        }
        if (dut_xor != expected_xor) {
            std::cout << "Fixed test error: in = all ones, expected out_xor = " 
                      << expected_xor << ", got = " << dut_xor << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    // 3. A checkerboard pattern: alternating 1 and 0.
    {
        ap_uint<100> in = 0;
        for (int i = 0; i < 100; i++) {
            if (i % 2 == 0) in[i] = 1;
        }
        bool dut_and, dut_or, dut_xor;
        TopModule(in, dut_and, dut_or, dut_xor);
        // out_and: false (not all ones)
        bool expected_and = false;
        // out_or: true (at least one 1)
        bool expected_or = true;
        // out_xor: XOR reduction of alternating pattern.
        // Since there are 50 ones and 50 zeros, XOR reduction is 0 (even number of 1's).
        bool expected_xor = false;
        if (dut_and != expected_and) {
            std::cout << "Fixed test error: checkerboard, expected out_and = " 
                      << expected_and << ", got = " << dut_and << std::endl;
            errorCount++;
        }
        if (dut_or != expected_or) {
            std::cout << "Fixed test error: checkerboard, expected out_or = " 
                      << expected_or << ", got = " << dut_or << std::endl;
            errorCount++;
        }
        if (dut_xor != expected_xor) {
            std::cout << "Fixed test error: checkerboard, expected out_xor = " 
                      << expected_xor << ", got = " << dut_xor << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Random Test Sequence ---
    std::srand((unsigned)std::time(nullptr));
    for (int i = 0; i < randomCases; i++) {
        // Generate a random 100-bit value.
        ap_uint<100> in = 0;
        for (int j = 0; j < 100; j++) {
            in[j] = (std::rand() % 2);
        }
        bool dut_and, dut_or, dut_xor;
        TopModule(in, dut_and, dut_or, dut_xor);

        bool expected_and = true;
        for (int j = 0; j < 100; j++) {
            expected_and = expected_and && in[j];
        }

        bool expected_or = false;
        for (int j = 0; j < 100; j++) {
            expected_or = expected_or || in[j];
        }

        bool expected_xor = false;
        for (int j = 0; j < 100; j++) {
            expected_xor ^= in[j];
        }

        if (dut_and != expected_and) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": in = " << in 
                      << ", expected out_and = " << expected_and 
                      << ", got = " << dut_and << std::endl;
            errorCount++;
        }
        if (dut_or != expected_or) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": in = " << in 
                      << ", expected out_or = " << expected_or 
                      << ", got = " << dut_or << std::endl;
            errorCount++;
        }
        if (dut_xor != expected_xor) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": in = " << in 
                      << ", expected out_xor = " << expected_xor 
                      << ", got = " << dut_xor << std::endl;
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
