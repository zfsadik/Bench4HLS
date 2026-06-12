#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8>& out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // --- Fixed Test Vectors ---
    ap_uint<8> testValues[8] = { 0x01, 0x02, 0x04, 0x08, 0x80, 0xC0, 0xE0, 0xF0 };
    for (int i = 0; i < 8; i++) {
        ap_uint<8> in = testValues[i];
        unsigned int in_val = in.to_uint();
        ap_uint<8> expected = ((in_val & 0x01u) << 7) | ((in_val & 0x02u) << 5) |
                              ((in_val & 0x04u) << 3) | ((in_val & 0x08u) << 1) |
                              ((in_val & 0x10u) >> 1) | ((in_val & 0x20u) >> 3) |
                              ((in_val & 0x40u) >> 5) | ((in_val & 0x80u) >> 7);
        ap_uint<8> out;
        TopModule(in, out);
        if (out != expected) {
            std::cout << "Error (fixed) at cycle " << cycleCount 
                      << ": in = " << std::hex << in 
                      << ", expected = " << expected 
                      << ", got = " << out << std::dec << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Vectors ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const int randomTests = 200;
    for (int i = 0; i < randomTests; i++) {
        ap_uint<8> in = std::rand() & 0xFF;  // Generate an 8-bit random value.
        unsigned int in_val = in.to_uint();
        ap_uint<8> expected = ((in_val & 0x01u) << 7) | ((in_val & 0x02u) << 5) |
                              ((in_val & 0x04u) << 3) | ((in_val & 0x08u) << 1) |
                              ((in_val & 0x10u) >> 1) | ((in_val & 0x20u) >> 3) |
                              ((in_val & 0x40u) >> 5) | ((in_val & 0x80u) >> 7);
        ap_uint<8> out;
        TopModule(in, out);
        if (out != expected) {
            std::cout << "Error (random) at cycle " << cycleCount 
                      << ": in = " << std::hex << in 
                      << ", expected = " << expected 
                      << ", got = " << out << std::dec << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Summary report.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
