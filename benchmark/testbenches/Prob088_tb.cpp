#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<6> y, ap_uint<1> w, ap_uint<1> &Y1, ap_uint<1> &Y3);

// Reference function to compute expected outputs.
void ref_compute(ap_uint<6> y, ap_uint<1> w, ap_uint<1> &Y1, ap_uint<1> &Y3) {
    // Derive next state from state machine table (A=bit0,B=bit1,C=bit2,D=bit3,E=bit4,F=bit5)
    ap_uint<6> next = 0;
    if (y[0]) next = w ? 0x02 : 0x01;  // A->B or A->A
    if (y[1]) next = w ? 0x04 : 0x08;  // B->C or B->D
    if (y[2]) next = w ? 0x10 : 0x08;  // C->E or C->D
    if (y[3]) next = w ? 0x20 : 0x01;  // D->F or D->A
    if (y[4]) next = w ? 0x10 : 0x08;  // E->E or E->D
    if (y[5]) next = w ? 0x04 : 0x08;  // F->C or F->D
    Y1 = next[1];
    Y3 = next[3];
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT and reference outputs.
    ap_uint<1> Y1_dut, Y3_dut;
    ap_uint<1> Y1_ref, Y3_ref;

    // Input signals.
    ap_uint<6> y;
    ap_uint<1> w;

    // --- Phase 1: One-Hot Test Cases (200 iterations) ---
    for (int i = 0; i < 200; i++) {
        // Create a one-hot value: randomly select one bit from 0 to 5.
        int index = std::rand() % 6;
        y = 1 << index;
        // Generate random w.
        w = std::rand() & 1;

        // Call DUT and reference function.
        TopModule(y, w, Y1_dut, Y3_dut);
        ref_compute(y, w, Y1_ref, Y3_ref);

        // Check each output for mismatch.
        if (Y1_dut != Y1_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << Y1_ref
                      << ", got " << Y1_dut << std::endl;
            errorCount++;
        }
        if (Y3_dut != Y3_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << Y3_ref
                      << ", got " << Y3_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Semi-Random Test Cases (400 iterations) ---
    for (int i = 0; i < 400; i++) {
        int temp;
        // Loop until a valid value is generated.
        do {
            temp = std::rand() % 64;  // 6-bit random number [0, 63]
        } while (__builtin_popcount(temp) != 1);
        y = temp;
        w = std::rand() & 1;

        TopModule(y, w, Y1_dut, Y3_dut);
        ref_compute(y, w, Y1_ref, Y3_ref);

        if (Y1_dut != Y1_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << Y1_ref
                      << ", got " << Y1_dut << std::endl;
            errorCount++;
        }
        if (Y3_dut != Y3_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << Y3_ref
                      << ", got " << Y3_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
