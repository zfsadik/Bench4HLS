#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool load, ap_uint<512> data, ap_uint<512> & q);

ap_uint<512> rule110(ap_uint<512> X) {
    const int table[8] = {0, 1, 1, 1, 0, 1, 1, 0};
    ap_uint<512> next = 0;
    for (int i = 0; i < 512; i++) {
        int left   = (i < 511) ? (int)X[i+1] : 0;
        int center = (int)X[i];
        int right  = (i > 0)   ? (int)X[i-1] : 0;
        next[i] = table[(left << 2) | (center << 1) | right];
    }
    return next;
}

void simulateCycle(bool load, ap_uint<512> data, ap_uint<512> &q_dut, ap_uint<512> &ref_state, int &cycleCount, int &errorCount) {
    TopModule(load, data, q_dut);

    // Update reference state.
    if (load)
        ref_state = data;
    else
        ref_state = rule110(ref_state);

    // Compare DUT output with reference.
    if (q_dut != ref_state) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << ref_state
                  << ", got " << q_dut << std::endl;
        errorCount++;
    }
    cycleCount++;
}

// Helper to generate a random 512-bit number.
ap_uint<512> random_512bit() {
    ap_uint<512> rand_val = 0;
    // For each bit, randomly set true/false.
    for (int i = 0; i < 512; i++) {
        if (rand() % 2)
            rand_val[i] = 1;
        else
            rand_val[i] = 0;
    }
    return rand_val;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool load = false;            // Load control signal
    ap_uint<512> data = 0;        // Data input
    ap_uint<512> q_dut = 0;       // DUT (design under test) output
    ap_uint<512> ref_state = 0;   // Reference state for simulation

    // --- Phase 1: Load initial state with a single 1 at bit 0 ---
    data = 0;
    data[0] = 1;  // Set bit 0
    load = true;
    // Perform 3 cycles with load asserted.
    for (int i = 0; i < 3; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }
    // Deassert load and simulate 10 cycles.
    load = false;
    for (int i = 0; i < 10; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }

    // --- Phase 2: Load state with a single 1 at bit 256 ---
    data = 0;
    data[256] = 1;
    load = true;
    // 3 cycles with load asserted.
    for (int i = 0; i < 3; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }
    // Deassert load and simulate 1000 cycles.
    load = false;
    for (int i = 0; i < 1000; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }

    // --- Phase 3: Load state with hexadecimal constant 4df ---
    // Note: 4df is a small number. It will appear in the lower bits.
    data = 0;
    data = 0x4df;
    load = true;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    load = false;
    for (int i = 0; i < 1000; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }

    // --- Phase 4: Load random data ---
    data = random_512bit();
    load = true;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    load = false;
    for (int i = 0; i < 1000; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }

    // --- Phase 5: Sequence with changing data ---
    data = 0;
    load = true;
    // Hold load asserted for 20 cycles.
    for (int i = 0; i < 20; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }
    // Change data in successive cycles.
    // Cycle: data = 2.
    data = 2;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    // Cycle: data = 4.
    data = 4;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    // Cycle: data = 9 and deassert load.
    data = 9;
    load = false;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    // Cycle: data = 12.
    data = 12;
    simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    // Then simulate 100 cycles without further data changes.
    for (int i = 0; i < 100; i++) {
        simulateCycle(load, data, q_dut, ref_state, cycleCount, errorCount);
    }

    // --- Final Test Summary ---
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
