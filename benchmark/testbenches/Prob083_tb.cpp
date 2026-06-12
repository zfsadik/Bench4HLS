#include <iostream>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<5>& q);

// Hardcoded 31-state maximal-length sequence for 5-bit Galois LFSR (taps 5,3), starting from reset state 1.
static const ap_uint<5> golden[31] = {
    1, 20, 10, 5, 22, 11, 17, 28, 14, 7, 23, 31, 27, 25, 24,
    12, 6, 3, 21, 30, 15, 19, 29, 26, 13, 18, 9, 16, 8, 4, 2
};

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    ap_uint<5> q;

    // Apply synchronous reset for 2 cycles; output must be golden[0] = 1
    for (int i = 0; i < 2; i++) {
        TopModule(true, q);
        if (q != golden[0]) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << golden[0] << ", got " << q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Verify the 31-state sequence over two full repetitions
    for (int i = 0; i < 62; i++) {
        TopModule(false, q);
        ap_uint<5> expected = golden[(i + 1) % 31];
        if (q != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << expected << ", got " << q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Verify mid-sequence synchronous reset
    TopModule(false, q);
    cycleCount++;
    TopModule(true, q);
    if (q != golden[0]) {
        std::cout << "Mid-sequence reset mismatch at cycle " << cycleCount
                  << ": expected " << golden[0] << ", got " << q << std::endl;
        errorCount++;
    }
    cycleCount++;

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    return 0;
}
