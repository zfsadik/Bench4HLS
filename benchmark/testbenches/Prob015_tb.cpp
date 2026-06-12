#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<16> in, ap_uint<8> &out_hi, ap_uint<8> &out_lo);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Run 110 test cycles.
    const int totalCycles = 110;
    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 16-bit input.
        ap_uint<16> in = std::rand() % 0x10000;

        ap_uint<8> out_hi, out_lo;
        TopModule(in, out_hi, out_lo);

        ap_uint<8> expected_hi = in >> 8;      // Upper 8 bits.
        ap_uint<8> expected_lo = in & 0xFF;      // Lower 8 bits.

        // Check for mismatches.
        if (out_hi != expected_hi || out_lo != expected_lo) {
            std::cout << "Error at cycle " << cycleCount
                      << ": in = 0x" << std::hex << in
                      << ", expected out_hi = 0x" << (unsigned)expected_hi
                      << ", got out_hi = 0x" << (unsigned)out_hi
                      << "; expected out_lo = 0x" << (unsigned)expected_lo
                      << ", got out_lo = 0x" << (unsigned)out_lo << std::dec
                      << std::endl;
            errorCount++;
        }
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
