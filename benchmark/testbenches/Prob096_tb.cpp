#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<6> y, bool w, bool &Y2, bool &Y4);

// Reference function that computes the expected outputs.
void ref_compute(ap_uint<6> y, bool w, bool &ref_Y2, bool &ref_Y4) {
    ref_Y2 = false;
    ref_Y4 = false;
    if      (y == 0x01) { if (!w) ref_Y2 = true; }  // A --0--> B
    else if (y == 0x02) { if ( w) ref_Y4 = true; }  // B --1--> D
    else if (y == 0x04) { if ( w) ref_Y4 = true; }  // C --1--> D
    else if (y == 0x10) { if ( w) ref_Y4 = true; }  // E --1--> D
    else if (y == 0x20) { if ( w) ref_Y4 = true; }  // F --1--> D
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT and reference outputs.
    bool Y2_dut = false, Y4_dut = false;
    bool Y2_ref = false, Y4_ref = false;

    // Input signals.
    ap_uint<6> y;
    bool w;

    // --- Phase 1: Fixed Test Cases ---
    // Fixed 6-bit patterns and w values: y = 0x01, 0x02, 0x04, 0x08, 0x10, 0x20 (one-hot values) and w = 0 or 1.
    ap_uint<6> fixedY[6] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20 };
    bool fixedW[2] = { false, true };

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 2; j++) {
            y = fixedY[i];
            w = fixedW[j];
            TopModule(y, w, Y2_dut, Y4_dut);
            ref_compute(y, w, Y2_ref, Y4_ref);
            if (Y2_dut != Y2_ref) {
                std::cout << "Mismatch at cycle " << cycleCount 
                          << ": expected Y2 = " << Y2_ref 
                          << ", got " << Y2_dut << std::endl;
                errorCount++;
            }
            if (Y4_dut != Y4_ref) {
                std::cout << "Mismatch at cycle " << cycleCount 
                          << ": expected Y4 = " << Y4_ref 
                          << ", got " << Y4_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // --- Phase 2: Random Test Cases ---
    // Apply 200 random test cases.
    for (int i = 0; i < 200; i++) {
        // Generate a random one-hot 6-bit value (valid FSM state encoding).
        y = ap_uint<6>(1) << (std::rand() % 6);
        // Generate a random value for w.
        w = (std::rand() & 1) ? true : false;
        
        TopModule(y, w, Y2_dut, Y4_dut);
        ref_compute(y, w, Y2_ref, Y4_ref);
        
        if (Y2_dut != Y2_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected Y2 = " << Y2_ref 
                      << ", got " << Y2_dut << std::endl;
            errorCount++;
        }
        if (Y4_dut != Y4_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected Y4 = " << Y4_ref 
                      << ", got " << Y4_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print the test summary.
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
