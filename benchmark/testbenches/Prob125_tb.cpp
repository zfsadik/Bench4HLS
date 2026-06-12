#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool sresetn, bool x, bool & z);

// Independent shift-register reference model for Mealy "101" detector.
// State encoded as last two inputs: prev2=older, prev1=newer.
// S10 (state=2) corresponds to prev2=1, prev1=0.
// Output z=1 iff in S10 and x=1, computed before state update.
bool ref_model(bool sresetn, bool x, bool &prev1, bool &prev2) {
    bool z_ref = (prev2 && !prev1) ? x : false;
    if (!sresetn) {
        prev1 = false;
        prev2 = false;
    } else {
        prev2 = prev1;
        prev1 = x;
    }
    return z_ref;
}

void simulateCycle(bool sresetn, bool x,
                   bool & z_dut, bool & prev1, bool & prev2,
                   int & cycleCount, int & errorCount) {
    TopModule(sresetn, x, z_dut);

    bool z_ref = ref_model(sresetn, x, prev1, prev2);

    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool sresetn;
    bool x;
    bool z_dut;
    bool prev1 = false, prev2 = false;

    // --- Phase 1: Specific Sequence Test ---
    sresetn = false; // Reset asserted (active low)
    x = false;       // x can be arbitrary during reset.
    for (int i = 0; i < 3; i++) {
        simulateCycle(sresetn, x, z_dut, prev1, prev2, cycleCount, errorCount);
    }
    // Deassert reset.
    sresetn = true;
    // Apply a specific sequence to force the "101" detection:
    // For example: x = 1, then x = 0, then x = 1.
    x = true;
    simulateCycle(sresetn, x, z_dut, prev1, prev2, cycleCount, errorCount);
    x = false;
    simulateCycle(sresetn, x, z_dut, prev1, prev2, cycleCount, errorCount);
    x = true;
    simulateCycle(sresetn, x, z_dut, prev1, prev2, cycleCount, errorCount);

    // --- Phase 2: Random Testing for 1000 Cycles ---
    for (int i = 0; i < 1000; i++) {
        x = (rand() % 2 == 1);
        // Random reset: approximate 1 in 16 chance that sresetn is low.
        sresetn = (rand() % 16 != 0);
        simulateCycle(sresetn, x, z_dut, prev1, prev2, cycleCount, errorCount);
    }

    // --- Final Test Summary ---
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
