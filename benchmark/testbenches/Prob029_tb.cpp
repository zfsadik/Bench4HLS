#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<255> in, ap_uint<8> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int cycle = 0; cycle < totalCycles; cycle++) {
        // Generate a random 255-bit input by setting each bit randomly.
        ap_uint<255> in_val = 0;
        for (int j = 0; j < 255; j++) {
            in_val[j] = (std::rand() % 2);
        }
        
        ap_uint<8> dut_out = 0;
        TopModule(in_val, dut_out);
        
        ap_uint<8> expected = 0;
        for (int j = 0; j < 255; j++) {
            expected += in_val[j];
        }
        
        // Compare DUT output with expected output.
        if (dut_out != expected) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": in = " << in_val 
                      << ", expected = " << (unsigned)expected 
                      << ", got = " << (unsigned)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Corner cases: all-zeros, all-ones, alternating pattern.
    {
        ap_uint<255> corner_in = 0;
        ap_uint<8> dut_out = 0, expected = 0;

        // All-zeros
        TopModule(corner_in, dut_out);
        if (dut_out != 0) { std::cout << "Mismatch: all-zeros, expected 0, got " << (unsigned)dut_out << std::endl; errorCount++; }
        cycleCount++;

        // All-ones
        corner_in = ~ap_uint<255>(0);
        expected = 255;
        TopModule(corner_in, dut_out);
        if (dut_out != expected) { std::cout << "Mismatch: all-ones, expected 255, got " << (unsigned)dut_out << std::endl; errorCount++; }
        cycleCount++;

        // Alternating: bits 0,2,4,... set => 128 ones
        corner_in = 0;
        expected = 0;
        for (int j = 0; j < 255; j += 2) { corner_in[j] = 1; expected++; }
        TopModule(corner_in, dut_out);
        if (dut_out != expected) { std::cout << "Mismatch: alternating, expected " << (unsigned)expected << ", got " << (unsigned)dut_out << std::endl; errorCount++; }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
