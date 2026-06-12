#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Exhaustive test of all 256 input values.
    for (int i = 0; i < 256; i++) {
        ap_uint<8> in = i;
        ap_uint<8> out;
        TopModule(in, out);
        ap_uint<8> expected = (ap_uint<8>)(~in);
        if (out != expected) {
            std::cout << "Error at cycle " << cycleCount
                      << ": Input = " << in
                      << ", Expected = " << expected
                      << ", Got = " << out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
