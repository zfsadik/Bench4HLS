#include <iostream>
#include "ap_int.h"

// Declaration of the TopModule function.
void TopModule(ap_uint<1>& zero);

int main() {
    int errors = 0;

    ap_uint<1> zero;
    TopModule(zero);

    // Verify that the output is always 0.
    if (zero != 0) {
        std::cout << "Error: Expected 0, got " << zero << std::endl;
        errors++;
    }

    // Print the overall test results.
    if (errors == 0) {
        std::cout << "Test Passed: All cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errors << " mismatches detected." << std::endl;
    }

    return 0;
}
