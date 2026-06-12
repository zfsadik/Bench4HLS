#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_int<8> in, ap_int<32> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Corner cases: 0x00, 0x7F (max positive), 0x80 (most negative), 0xFF (-1)
    {
        int8_t corners[] = {(int8_t)0x00, (int8_t)0x7F, (int8_t)0x80, (int8_t)0xFF};
        for (int c = 0; c < 4; c++) {
            ap_int<8> in_val = corners[c];
            ap_int<32> dut_out = 0;
            TopModule(in_val, dut_out);
            ap_int<32> expected = (ap_int<32>) in_val;
            if (dut_out != expected) {
                std::cout << "Mismatch at corner case " << c
                          << ": in = " << in_val
                          << ", expected = " << expected
                          << ", got = " << dut_out << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 8-bit value.
        // Note: std::rand() returns a positive integer; we mask to get 8 bits.
        unsigned int rand_val = std::rand() & 0xFF;
        // Cast to ap_int<8> to interpret it as a signed value.
        ap_int<8> in_val = (int8_t)(rand_val);
        
        ap_int<32> dut_out = 0;
        TopModule(in_val, dut_out);
        
        ap_int<32> expected = (ap_int<32>) in_val;
        
        if (dut_out != expected) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": in = " << in_val 
                      << ", expected = " << expected 
                      << ", got = " << dut_out << std::endl;
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
