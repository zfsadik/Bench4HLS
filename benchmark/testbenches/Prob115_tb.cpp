#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1> in, ap_uint<1> reset, ap_uint<1> &out);

void ref_compute(ap_uint<1> in, ap_uint<1> reset, ap_uint<1> &ref_out, ap_uint<2> &ref_state) {
    ap_uint<2> next;
    if (ref_state == 0) {       // State A
        if (in)
            next = 1;
        else
            next = 0;
    }
    else if (ref_state == 1) {  // State B
        if (in)
            next = 1;
        else
            next = 2;
    }
    else if (ref_state == 2) {  // State C
        if (in)
            next = 3;
        else
            next = 0;
    }
    else {                      // State D (ref_state == 3)
        if (in)
            next = 1;
        else
            next = 2;
    }

    if (reset) {
        ref_state = 0;  // Reset to state A.
    }
    else {
        ref_state = next;
    }
    ref_out = (ref_state == 3);  // Output is 1 only in state D.
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<1> in;
    ap_uint<1> reset;
    ap_uint<1> out_dut = 0;
    ap_uint<1> out_ref = 0;

    // Reference model internal variables.
    ap_uint<2> ref_state = 0;  // Start in state A.

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        in = 0;
        reset = 1;
        TopModule(in, reset, out_dut);
        ref_compute(in, reset, out_ref, ref_state);
    }

    // Simulate for 200 cycles.
    const int totalCycles = 200;
    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus for input 'in'.
        in = (std::rand() & 1);
        // Generate reset: let it be asserted with a probability (e.g., 1/8).
        reset = ((std::rand() % 8) == 0);

        TopModule(in, reset, out_dut);
        ref_compute(in, reset, out_ref, ref_state);

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
