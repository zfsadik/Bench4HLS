#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool resetn, bool in, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 200; // Total test cycles

    // Reference model to simulate the expected behavior of the shift register.
    ap_uint<4> sr_ref = 0;
    bool expected_out = 0;
    bool dut_out = 0;

    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(false, false, dut_out);
        sr_ref = 0;
        expected_out = sr_ref[3];
        if (dut_out != expected_out) {
            std::cout << "Mismatch at reset cycle " << i
                      << ": expected out = " << expected_out
                      << ", got out = " << dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    for (int i = 0; i < totalCycles; i++) {
        // Generate random input signals.
        bool in_val = (std::rand() % 2) ? true : false;
        // Generate random active-low reset signal.
        bool resetn_val = (std::rand() % 4 != 0); // 75% chance to be high, 25% chance low

        TopModule(resetn_val, in_val, dut_out);

        if (!resetn_val)
            sr_ref = 0;
        else
            sr_ref = (sr_ref << 1) | in_val;
        expected_out = sr_ref[3];

        // Check DUT output.
        if (dut_out != expected_out) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": resetn = " << resetn_val
                      << ", in = " << in_val
                      << ", expected out = " << expected_out
                      << ", got out = " << dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
