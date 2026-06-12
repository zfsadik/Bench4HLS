#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8> &pedge);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total number of rising-edge test cycles

    // Reference model to simulate expected behavior.
    ap_uint<8> ref_d_last = 0;
    ap_uint<8> dut_out = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulation loop:
    // For each cycle, simulate a rising edge by calling TopModule once.
    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 8-bit value for in.
        ap_uint<8> in_val = std::rand() % 256;

        TopModule(in_val, dut_out);

        ap_uint<8> expected = in_val & ~ref_d_last;
        ref_d_last = in_val;

        // Compare DUT output with expected output.
        if (dut_out != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": in = " << in_val
                      << ", expected pedge = " << expected
                      << ", got = " << dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
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
