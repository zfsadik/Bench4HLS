#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<3> a, ap_uint<3> b, 
               ap_uint<3> &out_or_bitwise, bool &out_or_logical, 
               ap_uint<6> &out_not);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int i = 0; i < totalCycles; i++) {
        // Generate random 3-bit values for inputs a and b (0 to 7).
        ap_uint<3> a = std::rand() % 8;
        ap_uint<3> b = std::rand() % 8;

        // DUT outputs.
        ap_uint<3> dut_out_or_bitwise;
        bool dut_out_or_logical;
        ap_uint<6> dut_out_not;
        TopModule(a, b, dut_out_or_bitwise, dut_out_or_logical, dut_out_not);

        ap_uint<3> expected_or_bitwise = a | b;
        bool expected_or_logical = (a != 0) || (b != 0);
        ap_uint<3> expected_not_a = ~a;
        ap_uint<3> expected_not_b = ~b;
        ap_uint<6> expected_out_not = ((ap_uint<6>)expected_not_b << 3) | (ap_uint<6>)expected_not_a;

        // Check bitwise OR.
        if (dut_out_or_bitwise != expected_or_bitwise) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", expected out_or_bitwise = " << expected_or_bitwise 
                      << ", got = " << dut_out_or_bitwise << std::endl;
            errorCount++;
        }
        // Check logical OR.
        if (dut_out_or_logical != expected_or_logical) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", expected out_or_logical = " << expected_or_logical 
                      << ", got = " << dut_out_or_logical << std::endl;
            errorCount++;
        }
        // Check concatenated NOT output.
        if (dut_out_not != expected_out_not) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", expected out_not = " << expected_out_not 
                      << ", got = " << dut_out_not << std::endl;
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
