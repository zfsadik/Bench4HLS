#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, bool x, bool &z);

void ref_compute(bool reset, bool in, bool &ref_z, ap_uint<3> &ref_state) {
    ref_z = ((ref_state == 3) || (ref_state == 4));

    if (reset) {
        ref_state = 0; // Synchronous reset to A.
    } else {
        ap_uint<3> next;
        if (ref_state == 0) { // State A: 000
            if (in == true)
                next = 1; // A -> B when in==1
            else
                next = 0; // A -> A when in==0
        } else if (ref_state == 1) { // State B: 001
            if (in == true)
                next = 4; // B -> E when in==1
            else
                next = 1; // B -> B when in==0
        } else if (ref_state == 2) { // State C: 010
            if (in == true)
                next = 1; // C -> B when in==1
            else
                next = 2; // C -> C when in==0
        } else if (ref_state == 3) { // State D: 011
            if (in == true)
                next = 2; // D -> C when in==1
            else
                next = 1; // D -> B when in==0
        } else { // ref_state == 4, State E: 100
            if (in == true)
                next = 4; // E -> E when in==1
            else
                next = 3; // E -> D when in==0
        }
        ref_state = next;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT signals.
    bool x;
    bool reset;
    bool z_dut = false;

    // Reference signals.
    ap_uint<3> ref_state = 0; // Start in A.
    bool z_ref = false;

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(/*reset=*/true, false, z_dut);
        ref_compute(/*reset=*/true, false, z_ref, ref_state);
    }

    // Total number of simulation cycles.
    const int totalCycles = 200;

    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus.
        x = ((std::rand() & 1) != 0);
        // Assert reset with some probability (e.g., once every 8 cycles).
        reset = ((std::rand() % 8) == 0);

        TopModule(reset, x, z_dut);
        ref_compute(reset, x, z_ref, ref_state);

        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
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
