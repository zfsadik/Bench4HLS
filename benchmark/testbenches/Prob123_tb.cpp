#include <iostream>
#include <cstdlib>

void TopModule(bool reset, bool bump_left, bool bump_right,
               bool & walk_left, bool & walk_right);

bool ref_update(bool reset, bool bump_left, bool bump_right, bool ref_state) {
    if (reset)
        return false;  // Reset to Walk Left
    else {
        if (!ref_state)  // Currently Walk Left
            return bump_left ? true : false;
        else             // Currently Walk Right
            return bump_right ? false : true;
    }
}

void simulateCycle(bool reset, bool bump_left, bool bump_right,
                   bool & walk_left_dut, bool & walk_right_dut,
                   bool & ref_state,
                   int & cycleCount, int & errorCount) {
    TopModule(reset, bump_left, bump_right, walk_left_dut, walk_right_dut);
    bool expected_walk_left = !ref_state;
    bool expected_walk_right = ref_state;
    // Update reference state on rising edge.
    ref_state = ref_update(reset, bump_left, bump_right, ref_state);
    // Compare DUT outputs with expected outputs.
    if ( (walk_left_dut != expected_walk_left) || (walk_right_dut != expected_walk_right) ) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected (walk_left, walk_right)=(" << expected_walk_left
                  << ", " << expected_walk_right << "), got ("
                  << walk_left_dut << ", " << walk_right_dut << ")" << std::endl;
        errorCount++;
    }
    cycleCount++;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool reset;                 // Reset signal
    bool bump_left, bump_right;  // Input bump signals
    bool walk_left_dut, walk_right_dut;  // DUT outputs
    bool ref_state = false;      // Reference state (initially Walk Left)

    // ---- Phase 1: Reset Test with Predetermined Stimuli ----
    reset = true;
    bump_left = true;
    bump_right = true;
    for (int i = 0; i < 3; i++) {
        simulateCycle(reset, bump_left, bump_right, walk_left_dut, walk_right_dut, ref_state, cycleCount, errorCount);
    }
    // Deassert reset.
    reset = false;
    // Apply a sequence: For two cycles, set bump_left = true (forcing a switch from WL to WR when in WL).
    bump_left = true;
    bump_right = false;
    for (int i = 0; i < 2; i++) {
        simulateCycle(reset, bump_left, bump_right, walk_left_dut, walk_right_dut, ref_state, cycleCount, errorCount);
    }
    // Then, for two cycles, set bump_left = false and bump_right = true (forcing a switch from WR to WL when in WR).
    bump_left = false;
    bump_right = true;
    for (int i = 0; i < 2; i++) {
        simulateCycle(reset, bump_left, bump_right, walk_left_dut, walk_right_dut, ref_state, cycleCount, errorCount);
    }

    // ---- Phase 2: Random Testing for 200 Cycles ----
    for (int i = 0; i < 200; i++) {
        // Generate random bump signals.
        bump_left = (rand() % 2 == 1);
        bump_right = (rand() % 2 == 1);
        // Randomly assert reset with roughly 1-in-32 chance.
        reset = (rand() % 32 == 0);
        simulateCycle(reset, bump_left, bump_right, walk_left_dut, walk_right_dut, ref_state, cycleCount, errorCount);
    }

    // ---- Final Test Summary ----
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
