#include <iostream>
#include "ap_int.h"

// Declaration of the TopModule function.
void TopModule(ap_uint<1>& one);

int main() {
    int errors = 0;

    ap_uint<1> dut_one;
    TopModule(dut_one);

    // Verify that the output is always 1.
    if (dut_one != 1) {
        std::cout << "Error: Expected 1, got " << dut_one << std::endl;
        errors++;
    }

    // Report the test result.
    if (errors == 0) {
        std::cout << "Test Passed: Output matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errors << " mismatches detected." << std::endl;
    }

    return 0;
}
