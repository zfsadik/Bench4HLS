#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<1> in1, ap_uint<1> in2, ap_uint<1> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            ap_uint<1> in1 = i;
            ap_uint<1> in2 = j;
            ap_uint<1> out_dut = 0;

            TopModule(in1, in2, out_dut);

            ap_uint<1> expected = in1 & (~in2);

            if (out_dut != expected) {
                std::cout << "Error at cycle " << cycleCount
                          << ": in1 = " << in1
                          << ", in2 = " << in2
                          << ", expected = " << expected
                          << ", got = " << out_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
