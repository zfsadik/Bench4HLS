#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<2> A, ap_uint<2> B, bool &z);

int main() {
    int errorCount = 0;
    int totalCycles = 0;

    for (int a = 0; a < 4; a++) {
        for (int b = 0; b < 4; b++) {
            ap_uint<2> A = a;
            ap_uint<2> B = b;
            bool z_dut = false;

            TopModule(A, B, z_dut);

            bool expected = (A == B);

            if (z_dut != expected) {
                std::cout << "Mismatch at cycle " << totalCycles
                          << ": A = " << A
                          << ", B = " << B
                          << ", expected z = " << expected
                          << ", got z = " << z_dut << std::endl;
                errorCount++;
            }
            totalCycles++;
        }
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << totalCycles << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << totalCycles << " cases." << std::endl;
    }

    return 0;
}
