#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool in, bool reset, bool &out);

void ref_compute(bool in, bool reset, bool &ref_out, ap_uint<2> &ref_state) {
    ref_out = (ref_state == 3);

    ap_uint<2> next;
    if (ref_state == 0) {         // State A
        if (in == true)
            next = 1;           // A --(in=1)--> B
        else
            next = 0;           // A --(in=0)--> A
    } else if (ref_state == 1) {    // State B
        if (in == true)
            next = 1;           // B --(in=1)--> B
        else
            next = 2;           // B --(in=0)--> C
    } else if (ref_state == 2) {    // State C
        if (in == true)
            next = 3;           // C --(in=1)--> D
        else
            next = 0;           // C --(in=0)--> A
    } else {                      // ref_state == 3, State D
        if (in == true)
            next = 1;           // D --(in=1)--> B
        else
            next = 2;           // D --(in=0)--> C
    }

    if (reset) {
        ref_state = 0; // Reset to state A.
    } else {
        ref_state = next;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT signals.
    bool in;
    bool reset;
    bool out_dut = false;

    // Reference model variables.
    ap_uint<2> ref_state = 0;  // Start in state A.
    bool out_ref = false;

    const int totalCycles = 200;

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(false, true, out_dut);
        ref_compute(false, true, out_ref, ref_state);
    }

    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus.
        in = ((std::rand() & 1) != 0);
        // Assert reset with some probability (e.g., 1/8).
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
