#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool x, bool &z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    bool dut_z;
    bool x;
    bool z_exp;

    ap_uint<3> goldenState = 0;  // Initially all flip-flops are 0.

    // Function to update the golden state on a rising edge.
    auto updateGoldenState = [&](bool x_in) {
        // Expected output is NOR of the three bits (current state before update).
        z_exp = !(goldenState[2] || goldenState[1] || goldenState[0]);
        // Compute new bits.
        bool new_bit2 = (bool)(goldenState[2]) ^ x_in;
        bool new_bit1 = (!((bool)goldenState[1])) && x_in;
        bool new_bit0 = (!((bool)goldenState[0])) || x_in;
        // Build new state.
        ap_uint<3> newState = 0;
        newState[2] = new_bit2;
        newState[1] = new_bit1;
        newState[0] = new_bit0;
        goldenState = newState;
    };

    // Test Sequence 1: Fixed pattern
    // Drive a known pattern: first four cycles x = false, next four cycles x = true.
    bool fixedPattern[8] = { false, false, false, false, true, true, true, true };
    for (int i = 0; i < 8; i++) {
        x = fixedPattern[i];
        TopModule(x, dut_z);
        // On rising edge update the golden model.
        updateGoldenState(fixedPattern[i]);
        // Compare the design output with expected output.
        if (dut_z != z_exp) {
            std::cout << "Error at test case " << cycleCount
                      << ": input x = " << (int)fixedPattern[i]
                      << " expected z = " << (int)z_exp
                      << ", got z = " << (int)dut_z << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Test Sequence 2: Random test cases (100 cycles)
    for (int i = 0; i < 100; i++) {
        // Generate a random boolean for x.
        bool rand_x = (std::rand() % 2) ? true : false;
        x = rand_x;
        TopModule(x, dut_z);
        updateGoldenState(rand_x);
        if (dut_z != z_exp) {
            std::cout << "Error at test case " << cycleCount
                      << ": input x = " << (int)rand_x
                      << " expected z = " << (int)z_exp
                      << ", got z = " << (int)dut_z << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
