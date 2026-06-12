#include <iostream>

void TopModule(bool in1, bool in2, bool &out);

int main() {
    int errorCount = 0;

    // Exhaustive truth-table sweep over all 4 input combinations.
    for (int i = 0; i < 4; i++) {
        bool in1 = (i >> 1) & 1;
        bool in2 = (i >> 0) & 1;
        bool out_dut = false;

        TopModule(in1, in2, out_dut);

        bool expected = !(in1 || in2);

        // Compare the design output with the expected result.
        if (out_dut != expected) {
            std::cout << "Error at case " << i
                      << ": in1 = " << in1
                      << ", in2 = " << in2
                      << ", expected out = " << expected
                      << ", got out = " << out_dut << std::endl;
            errorCount++;
        }
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All 4 test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of 4 cases." << std::endl;
    }

    return 0;
}
