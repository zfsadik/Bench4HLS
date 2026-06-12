#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, bool &shift_ena);

#define B0 0
#define B1 1
#define B2 2
#define B3 3
#define DONE 4

// Reference FSM model (should mimic the DUT FSM behavior).
void refFSM(bool reset, bool &shift_ena, ap_uint<3> &ref_state) {
    if (reset) {
        ref_state = B0;
    } else {
        switch(ref_state) {
            case B0: ref_state = B1; break;
            case B1: ref_state = B2; break;
            case B2: ref_state = B3; break;
            case B3: ref_state = DONE; break;
            case DONE: ref_state = DONE; break;
            default: ref_state = DONE; break;
        }
    }
    // Reference output is true if state < DONE.
    shift_ena = (ref_state < DONE);
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool reset = false;
    bool shift_ena_dut = false;
    bool shift_ena_ref = false;

    // Variables for the reference FSM.
    ap_uint<3> ref_state = DONE;

    // Total simulation cycles.
    const int totalCycles = 100;

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(true, shift_ena_dut);
        refFSM(true, shift_ena_ref, ref_state);
    }

    // Simulation loop.
    for (int i = 0; i < totalCycles; i++) {
        // Generate random reset value for each cycle.
        // For example, with probability 1/32, set reset true.
        reset = ((std::rand() % 32) == 0);

        TopModule(reset, shift_ena_dut);
        refFSM(reset, shift_ena_ref, ref_state);

        // Compare DUT output with reference.
        if (shift_ena_dut != shift_ena_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << shift_ena_ref
                      << ", got " << shift_ena_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
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
