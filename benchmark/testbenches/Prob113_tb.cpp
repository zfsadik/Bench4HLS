#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool a, ap_uint<3> &q);

// Reference function: implements the same sequential behavior.
void ref_compute(bool a, ap_uint<3> &q_ref, ap_uint<3> &reg_ref) {

    if (a) {
        reg_ref = 4; // Load preset value when a is high.
    } else if (reg_ref == 6) {
        reg_ref = 0; // Wrap around to 0 if current value is 6.
    } else {
        reg_ref = reg_ref + 1; // Otherwise, increment.
    }
    q_ref = reg_ref;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool a;
    ap_uint<3> q_dut = 0;

    // Reference model variables.
    ap_uint<3> q_ref = 0;
    ap_uint<3> reg_ref = 0;

    const int totalCycles = 200;

    // Phase 1: Use a fixed pattern following the given waveform.
    {
        // Cycle 0: Let a = 1 so that on the first rising edge q becomes 4.
        a = true;
        TopModule(a, q_dut);
        ref_compute(a, q_ref, reg_ref);
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Phase 2: Apply additional cycles with random stimulus.
    for (int i = 0; i < totalCycles - 1; i++) {
        // Generate random stimulus for input a.
        // For instance, let a be true for about 50% of the cycles.
        a = ((std::rand() & 1) != 0);
        TopModule(a, q_dut);
        ref_compute(a, q_ref, reg_ref);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Final test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
