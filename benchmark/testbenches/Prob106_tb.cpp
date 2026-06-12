#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool j, bool k, bool reset, bool &out);

void ref_compute(bool j, bool k, bool reset, bool &ref_out, bool &ref_state) {
    bool cur_state = ref_state;
    if (reset) {
        ref_state = false;
    }
    else {
        if (ref_state == false) {  // Currently OFF.
            if (j == true) {
                ref_state = true;  // Transition to ON if j==1.
            } else {
                ref_state = false;
            }
        }
        else {  // Currently ON.
            if (k == true) {
                ref_state = false; // Transition to OFF if k==1.
            } else {
                ref_state = true;
            }
        }
    }
    ref_out = cur_state;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool j;
    bool k;
    bool reset;
    bool out_dut = false;
    bool out_ref = false;

    // Variables for the reference FSM.
    bool ref_state = false;     // Assume initial state is OFF.

    // Apply reset for ≥2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(false, false, true, out_dut);
        ref_compute(false, false, true, out_ref, ref_state);
    }

    const int totalCycles = 200;  // Number of simulation cycles.
    for (int i = 0; i < totalCycles; i++) {
        j = ((std::rand() & 1) != 0);
        k = ((std::rand() & 1) != 0);
        // Reset is high with about 25% probability.
        reset = ((std::rand() % 4) == 0);

        TopModule(j, k, reset, out_dut);
        ref_compute(j, k, reset, out_ref, ref_state);

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
