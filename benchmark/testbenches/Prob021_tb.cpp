#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1024> in, ap_uint<8> sel, ap_uint<4> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 1000;  // Run 1000 test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int cycle = 0; cycle < totalCycles; cycle++) {
        // Generate a random 1024-bit input.
        ap_uint<1024> in_val = 0;
        for (int j = 0; j < 32; j++) {
            unsigned int chunk = std::rand();
            in_val |= ((ap_uint<1024>)chunk) << (32 * j);
        }

        // Generate a random 8-bit selector (0 to 255).
        ap_uint<8> sel_val = std::rand() % 256;

        ap_uint<4> dut_out = 0;

        TopModule(in_val, sel_val, dut_out);

        int start_index = sel_val * 4;
        ap_uint<4> expected = 0;
        for (int k = 0; k < 4; k++) {
            expected[k] = in_val[start_index + k];
        }

        // Compare the DUT output with the expected result.
        if (dut_out != expected) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": sel = " << sel_val 
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
