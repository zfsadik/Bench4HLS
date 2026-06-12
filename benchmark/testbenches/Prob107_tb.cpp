#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool j, bool k, bool reset, bool &out);

void ref_compute(bool j, bool k, bool reset, bool &ref_out, bool &ref_state) {
    ref_out = ref_state;
    if (reset) {
        ref_state = false;  // Reset forces state to OFF.
    }
    else {
        if (ref_state == false) { // Currently OFF
            if (j == true) {
                ref_state = true;  // Transition to ON.
            }
            else {
                ref_state = false; // Remain OFF.
            }
        }
        else { // Currently ON
            if (k == true) {
                ref_state = false; // Transition to OFF.
            }
            else {
                ref_state = true;  // Remain ON.
            }
        }
    }
}

int main() {
    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool j;
    bool k;
    bool reset;
    bool out_dut = false;
    bool out_ref = false;

    // Reference model variables.
    bool ref_state = false;     // Assume initial state is OFF.

    for (int i = 0; i < 2; i++) {
        TopModule(false, false, true, out_dut);
        ref_compute(false, false, true, out_ref, ref_state);
    }

    const int totalCycles = 200;
    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus:
        j = ((std::rand() & 1) != 0);
        k = ((std::rand() & 1) != 0);
        // Reset is asserted with ~25% probability.
        reset = ((std::rand() % 4) == 0);

        TopModule(j, k, reset, out_dut);
        ref_compute(j, k, reset, out_ref, ref_state);

        // Compare DUT output with reference.
        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << out_ref
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

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
