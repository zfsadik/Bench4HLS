#include <iostream>

// Declaration of the TopModule function.
void TopModule(bool x, bool y, bool &z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Exhaustive test of all 4 input combinations.
    for (int xi = 0; xi <= 1; xi++) {
        for (int yi = 0; yi <= 1; yi++) {
            bool x = (bool)xi;
            bool y = (bool)yi;

            bool z_dut = false;

            TopModule(x, y, z_dut);

            bool expected = (x ^ y) & x;

            // Compare the output.
            if (z_dut != expected) {
                std::cout << "Error at cycle " << cycleCount
                          << ": x = " << x
                          << ", y = " << y
                          << ", expected z = " << expected
                          << ", got z = " << z_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Report summary of test results.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
