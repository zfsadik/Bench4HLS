#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<32> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    ap_uint<32> golden_q = 1;

    ap_uint<32> dut_q = 0;

    // Simulation parameters.
    const int phase1_cycles = 400;
    const int phase2_cycles = 600;
    const int phase3_cycles = 5;
    const int total_cycles = phase1_cycles + phase2_cycles + phase3_cycles;

    for (int cycle = 0; cycle < total_cycles; cycle++) {
        bool reset_signal;
        // Phase 1: cycles 0..399: random reset.
        if (cycle < phase1_cycles) {
            // reset = true if (rand() & 31) equals 0.
            reset_signal = ((std::rand() & 31) == 0);
        }
        // Phase 2: cycles 400..999: force reset = false.
        else if (cycle < phase1_cycles + phase2_cycles) {
            reset_signal = false;
        }
        // Phase 3: cycles 1000..1004: force reset = true.
        else {
            reset_signal = true;
        }

        // Simulate a rising clock edge by calling TopModule with clk = true.
        TopModule(reset_signal, dut_q);

        if (reset_signal) {
            golden_q = 1;
        } else {
            ap_uint<32> q_next = golden_q >> 1; // shift right by one (zero-extended)
            q_next[31] = golden_q[0];           // new MSB = current LSB
            q_next[21] = q_next[21] ^ golden_q[0];
            q_next[1]  = q_next[1]  ^ golden_q[0];
            q_next[0]  = q_next[0]  ^ golden_q[0];
            golden_q = q_next;
        }

        // Compare DUT output with golden model.
        if (dut_q != golden_q) {
            std::cout << "Error at test case " << cycleCount 
                      << ": reset = " << reset_signal 
                      << ", expected q = 0x" << std::hex << golden_q 
                      << ", got q = 0x" << dut_q << std::dec << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " 
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
