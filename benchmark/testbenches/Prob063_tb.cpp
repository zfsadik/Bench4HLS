#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<32> in, ap_uint<32> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 200; // Total number of test cycles

    // Reference model variables:
    ap_uint<32> ref_d_last = 0;
    ap_uint<32> ref_out = 0;

    ap_uint<32> dut_out = 0;

    bool reset = false;

    // Seed the random generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(/*reset=*/true, (ap_uint<32>)0, dut_out);
        ref_out = 0;
        ref_d_last = 0;
    }

    // Run a fixed test sequence first.
    {
        // Test 1: Start with input all ones, then all zeros.
        ap_uint<32> test_in = 0xFFFFFFFF;
        // Force a synchronous reset.
        reset = true;
        TopModule(reset, test_in, dut_out);
        // Reference model update.
        ref_out = 0; // reset clears the captured bits
        ref_d_last = test_in;
        // Check DUT output.
        if (dut_out != ref_out) {
            std::cout << "Fixed test 1 error at cycle " << cycleCount
                      << ": test_in = " << (unsigned int)test_in
                      << ", reset = " << reset
                      << ", expected out = " << (unsigned int)ref_out
                      << ", got out = " << (unsigned int)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;

        // Test 2: Now remove reset and transition from all ones to all zeros.
        reset = false;
        test_in = 0x00000000;
        TopModule(reset, test_in, dut_out);
        // Reference model: previous d_last was all ones, now in is 0,
        // so event = (~0) & 0xFFFFFFFF = 0xFFFFFFFF. Therefore, ref_out becomes 0xFFFFFFFF.
        ref_out = ref_out | ((~test_in) & ref_d_last);
        ref_d_last = test_in;
        if (dut_out != ref_out) {
            std::cout << "Fixed test 2 error at cycle " << cycleCount
                      << ": test_in = " << (unsigned int)test_in
                      << ", reset = " << reset
                      << ", expected out = " << (unsigned int)ref_out
                      << ", got out = " << (unsigned int)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Run random test sequence.
    for (int i = 0; i < totalCycles; i++) {
        // Randomly decide whether to assert reset (with a 10% chance).
        reset = ((std::rand() % 10) == 0);
        // Generate a random 32-bit input.
        ap_uint<32> in_val = std::rand();
        in_val = (in_val << 16) | std::rand();  // combine two random 16-bit numbers

        TopModule(reset, in_val, dut_out);

        // Reference model update on rising edge:
        if (reset) {
            ref_out = 0;
        } else {
            ap_uint<32> event = ((~in_val) & ref_d_last);
            ref_out = ref_out | event;
        }
        ref_d_last = in_val;

        // Compare DUT output with reference.
        if (dut_out != ref_out) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": in = " << (unsigned int)in_val
                      << ", reset = " << reset
                      << ", expected out = " << (unsigned int)ref_out
                      << ", got out = " << (unsigned int)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
