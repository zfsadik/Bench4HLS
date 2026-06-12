#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<4> in, ap_uint<1> &out_and, ap_uint<1> &out_or, ap_uint<1> &out_xor);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCases = 16;  // All possible 4-bit combinations

    // --- Fixed Test Sequence ---
    for (int i = 0; i < fixedCases; i++) {
        ap_uint<4> in = i;  // 4-bit value from 0 to 15
        ap_uint<1> dut_and, dut_or, dut_xor;
        TopModule(in, dut_and, dut_or, dut_xor);

        // Expected outputs:
        ap_uint<1> expected_and = in.and_reduce();
        ap_uint<1> expected_or  = in.or_reduce();
        ap_uint<1> expected_xor = in.xor_reduce();

        if (dut_and != expected_and) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": in = " << in
                      << ", expected out_and = " << expected_and
                      << ", got = " << dut_and << std::endl;
            errorCount++;
        }
        if (dut_or != expected_or) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": in = " << in
                      << ", expected out_or = " << expected_or
                      << ", got = " << dut_or << std::endl;
            errorCount++;
        }
        if (dut_xor != expected_xor) {
            std::cout << "Fixed test error at cycle " << cycleCount
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
