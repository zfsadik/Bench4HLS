#include <iostream>
#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool d, bool e, ap_uint<25> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int i = 0; i < 32; i++) {
        bool a = (i >> 4) & 1;
        bool b = (i >> 3) & 1;
        bool c = (i >> 2) & 1;
        bool d = (i >> 1) & 1;
        bool e = i & 1;

        ap_uint<25> dut_out;
        TopModule(a, b, c, d, e, dut_out);

        bool in_arr[5] = { a, b, c, d, e };
        ap_uint<25> expected = 0;
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                int idx = 24 - (row * 5 + col);
                expected[idx] = (in_arr[row] == in_arr[col]) ? 1 : 0;
            }
        }

        // Compare DUT output with expected output.
        if (dut_out != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": inputs = {" << a << ", " << b << ", " << c << ", " << d << ", " << e << "}"
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
