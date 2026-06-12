#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<8> in, bool reset, bool & done);

ap_uint<2> ref_update(bool reset, ap_uint<8> in, ap_uint<2> state) {
    ap_uint<2> next_state;
    bool in3 = in[3];
    switch (state) {
        case 0: next_state = in3 ? 1 : 0; break;  // BYTE1
        case 1: next_state = 2; break;            // BYTE2
        case 2: next_state = 3; break;            // BYTE3
        case 3: next_state = in3 ? 1 : 0; break;  // DONE
        default: next_state = 0; break;
    }
    if (reset)
        return 0;  // Reset to BYTE1.
    else
        return next_state;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    ap_uint<8> in;
    bool reset;
    bool done_dut;  // Output from the design.
    bool done_ref;  // Expected output from the reference model.
    // Reference state (initially BYTE1, encoded as 0)
    ap_uint<2> ref_state = 0;

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        in = 0;
        reset = true;
        done_ref = (ref_state == 3);
        TopModule(in, reset, done_dut);
        ref_state = ref_update(reset, in, ref_state);
        if (done_dut != done_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << done_ref
                      << ", got " << done_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Test Phase: Continuous Random Testing ---
    for (int i = 0; i < 2000; i++) {
        // Generate random 8-bit input.
        in = rand() & 0xFF;
        // Randomly assert reset (approximately 1 in 32 chance).
        reset = (rand() % 32 == 0);

        done_ref = (ref_state == 3);
        TopModule(in, reset, done_dut);

        // Update the reference state.
        ref_state = ref_update(reset, in, ref_state);

        // Compare DUT and reference outputs.
        if (done_dut != done_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << done_ref
                      << ", got " << done_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Test Summary ---
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
