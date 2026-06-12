#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> d, ap_uint<8> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total number of test cycles

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ap_uint<8> prev_d = 0;

    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 8-bit input.
        ap_uint<8> d = std::rand() % 256;
        ap_uint<8> dut_q = 0;

        TopModule(d, dut_q);

        ap_uint<8> expected = prev_d;

        if (dut_q != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": d = " << d
                      << ", expected q = " << expected
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
        prev_d = d;
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
