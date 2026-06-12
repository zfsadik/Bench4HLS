#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool load, ap_uint<512> data, ap_uint<512> &q);

void ref_model(bool load, ap_uint<512> data, ap_uint<512> &q_ref,
               ap_uint<512> &reg_ref) {

    if (load) {
        reg_ref = data;
    } else {
        ap_uint<511> X = reg_ref.range(511, 1);  // q[511:1]
        ap_uint<511> Y = reg_ref.range(510, 0);  // q[510:0]
        ap_uint<512> X_ext = X;          // zero-extended MSB is 0.
        ap_uint<512> Y_ext = ((ap_uint<512>)Y) << 1;  // shift Y left by one.
        reg_ref = X_ext ^ Y_ext;
    }
    q_ref = reg_ref;
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<512> q_dut = 0;
    ap_uint<512> q_ref = 0;

    bool load;
    ap_uint<512> data;

    ap_uint<512> reg_ref = 0;

    const int totalCycles = 5000;  // Adjust for thorough testing.

    // Apply a series of stimulus cycles.
    for (int i = 0; i < totalCycles; i++) {
        // Phase 1: Load seed on cycle 0, then observe Rule 90 evolution for cycles 1-19.
        if (i == 0) {
            // Set bit 0 to draw a Sierpinski triangle starting from a single cell.
            data = 0;
            data[0] = 1;
            load = true;
        } else if (i < 20) {
            load = false;  // Let the automaton evolve.
        } else {
            // Then for subsequent cycles, use random data and random load (low probability for load).
            data = 0;
            // Generate a random 512-bit pattern by setting some bits randomly.
            uint32_t rnd = std::rand();
            for (int j = 0; j < 16; j++) {
                data.range((j+1)*32-1, j*32) = rnd;
            }
            load = ((std::rand() % 32) == 0);
        }

        TopModule(load, data, q_dut);
        ref_model(load, data, q_ref, reg_ref);

        // Compare DUT output with reference.
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print the final test summary.
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
