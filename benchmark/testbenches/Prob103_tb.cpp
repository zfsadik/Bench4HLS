#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool in, bool state, bool reset, bool &next_state, bool &out);

void ref_compute(bool in, bool state, bool reset, bool &next_state, bool &out) {
    out = state;
    if (reset) {
        next_state = true;
    } else {
        next_state = !(state ^ in);
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool in;
    bool reset;
    bool out_dut = false;
    bool out_ref = false;
    bool next_state_dut = false;
    bool next_state_ref = false;

    // Reference state variables.
    bool ref_state = false;      // Initially state A (false)

    // Apply reset for >=2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(false, ref_state, true, next_state_dut, out_dut);
        ref_compute(false, ref_state, true, next_state_ref, out_ref);
        ref_state = next_state_ref;
    }

    const int totalCycles = 200;  // Number of simulation cycles
    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus.
        in = (std::rand() & 1) ? true : false;
        // Reset asserted with lower probability.
        reset = ((std::rand() % 4) == 0);

        TopModule(in, ref_state, reset, next_state_dut, out_dut);
        ref_compute(in, ref_state, reset, next_state_ref, out_ref);

        if (out_dut != out_ref || next_state_dut != next_state_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out=" << out_ref << " next_state=" << next_state_ref
                      << ", got out=" << out_dut << " next_state=" << next_state_dut << std::endl;
            errorCount++;
        }

        ref_state = next_state_ref;
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
