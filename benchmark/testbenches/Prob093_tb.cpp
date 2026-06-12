#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

#define S     0
#define S1    1
#define S11   2
#define S110  3
#define DONE  4

void TopModule(bool reset, bool data, bool &start_shifting);

// Reference FSM: Mirrors the FSM in TopModule.
void refFSM(bool reset, bool data, bool &start_shifting, ap_uint<3> &ref_state) {
    ap_uint<3> cur_state = ref_state;
    if (reset) {
        ref_state = S;
    } else {
        switch(cur_state) {
            case S:
                ref_state = (data ? S1 : S);
                break;
            case S1:
                ref_state = (data ? S11 : S);
                break;
            case S11:
                ref_state = (data ? S11 : S110);
                break;
            case S110:
                ref_state = (data ? DONE : S);
                break;
            case DONE:
                ref_state = DONE;
                break;
            default:
                ref_state = DONE;
                break;
        }
    }
    // The reference output: start_shifting is high only when in DONE.
    start_shifting = (cur_state == DONE);
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool reset = false;
    bool data = false;
    bool start_shifting_dut = false;
    bool start_shifting_ref = false;

    // Variables for the reference FSM.
    ap_uint<3> ref_state = S;

    // Phase 1: Fixed Test Sequence.
    // Apply reset for 2 cycles.
    reset = true;
    data = true;  // Arbitrary.
    for (int i = 0; i < 2; i++) {
        TopModule(reset, data, start_shifting_dut);
        refFSM(reset, data, start_shifting_ref, ref_state);
        if (start_shifting_dut != start_shifting_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << start_shifting_ref
                      << ", got " << start_shifting_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Deassert reset.
    reset = false;

    // Now send a fixed sequence to search for 1101.
    // Example sequence: 1, 1, 0, 1 (which should lead to Done).
    bool fixedSeq[4] = { true, true, false, true };
    for (int i = 0; i < 4; i++) {
        data = fixedSeq[i];
        TopModule(reset, data, start_shifting_dut);
        refFSM(reset, data, start_shifting_ref, ref_state);
        if (start_shifting_dut != start_shifting_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << start_shifting_ref
                      << ", got " << start_shifting_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Phase 2: Random Test Cases.
    // Run for, say, 100 random cycles.
    const int randomCycles = 100;
    for (int i = 0; i < randomCycles; i++) {
        // Randomly generate data; also randomly assert reset with probability 1/32.
        reset = ((std::rand() % 32) == 0);
        data = ((std::rand() & 1) != 0);

        TopModule(reset, data, start_shifting_dut);
        refFSM(reset, data, start_shifting_ref, ref_state);
        if (start_shifting_dut != start_shifting_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << start_shifting_ref
                      << ", got " << start_shifting_dut << std::endl;
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
