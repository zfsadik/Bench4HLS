#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, bool x, bool &z);

/// Reference model of the Moore state machine for serial 2's complementer.
void ref_TopModule(bool reset, bool x, bool &z, ap_uint<2> &ref_state) {
    // Synchronous reset forces state A.
    if (reset) {
        ref_state = 0;
    }
    else {
        switch (ref_state) {
            case 0: // In state A.
                ref_state = (x ? 2 : 0);
                break;
            case 1: // In state B.
                ref_state = (x ? 1 : 2);
                break;
            case 2: // In state C.
                ref_state = (x ? 1 : 2);
                break;
            default:
                ref_state = 0;
                break;
        }
    }
    // Output z is 1 when state is C.
    z = (ref_state == 2);
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // Simulation signals.
    bool reset = false;
    bool x = false;
    bool z_dut = false;
    bool z_ref = false;

    // Variables for reference model.
    ap_uint<2> ref_state = 0;   // Start in state A.

    // --- Phase 1: Initialization ---
    // Cycles 0-1: Assert reset for 2 cycles.
    reset = true;
    x = false;
    for (int i = 0; i < 2; i++) {
        TopModule(reset, x, z_dut);
        ref_TopModule(reset, x, z_ref, ref_state);
        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Cycle 2: Deassert reset, set x = 1.
    reset = false;
    x = true;
    TopModule(reset, x, z_dut);
    ref_TopModule(reset, x, z_ref, ref_state);
    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Cycle 3: Set x = 0.
    x = false;
    TopModule(reset, x, z_dut);
    ref_TopModule(reset, x, z_ref, ref_state);
    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;

    // --- Phase 2: Reset Test Phase ---
    // First, deassert reset for a few cycles.
    reset = false;
    for (int i = 0; i < 3; i++) {
        TopModule(reset, x, z_dut);
        ref_TopModule(reset, x, z_ref, ref_state);
        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    // Then, assert reset.
    reset = true;
    TopModule(reset, x, z_dut);
    ref_TopModule(reset, x, z_ref, ref_state);
    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;
    // Deassert reset.
    reset = false;
    TopModule(reset, x, z_dut);
    ref_TopModule(reset, x, z_ref, ref_state);
    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;

    // --- Phase 3: Fixed Test Vectors ---
    // The following 9 test cases use predetermined {reset,x} values:
    //   2'h2 -> 10 -> reset=true, x=0
    //   2'h0 -> 00 -> reset=false, x=0
    //   2'h0 -> 00 -> reset=false, x=0
    //   2'h1 -> 01 -> reset=false, x=1
    //   2'h0 -> 00 -> reset=false, x=0
    //   2'h1 -> 01 -> reset=false, x=1
    //   2'h1 -> 01 -> reset=false, x=1
    //   2'h0 -> 00 -> reset=false, x=0
    //   2'h0 -> 00 -> reset=false, x=0
    int fixedSeq[9] = {0x2, 0x0, 0x0, 0x1, 0x0, 0x1, 0x1, 0x0, 0x0};
    for (int i = 0; i < 9; i++) {
        // Extract bits: MSB is reset, LSB is x.
        reset = (((fixedSeq[i] >> 1) & 1) != 0);
        x      = (((fixedSeq[i] >> 0) & 1) != 0);
        TopModule(reset, x, z_dut);
        ref_TopModule(reset, x, z_ref, ref_state);
        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 4: Random Test Vectors ---
    for (int i = 0; i < 400; i++) {
        // Generate random reset and x.
        // For reset: with probability 1/32, set reset true.
        reset = ((std::rand() & 31) == 0);
        x = ((std::rand() & 1) == 0) ? false : true;
        TopModule(reset, x, z_dut);
        ref_TopModule(reset, x, z_ref, ref_state);
        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print the test summary report.
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
