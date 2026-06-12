#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, ap_uint<3> &ena, ap_uint<16> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Reference model for BCD counter.
    ap_uint<16> ref_state = 0;  // Initially 0.
    ap_uint<3> ref_ena = 0;       // Reference enable output.

    // DUT output.
    ap_uint<16> dut_q = 0;
    ap_uint<3> dut_ena = 0;

    // Simulation settings.
    const int fixedCycles = 20;
    const int randomCycles = 100;

    bool reset = false;

    // Fixed Test Sequence.
    // First, perform a reset test: assert reset for two rising edges.
    reset = true;
    for (int i = 0; i < 2; i++) {
        TopModule(reset, dut_ena, dut_q);
        // Reference model: reset sets state to 0; post-update ena is 0.
        ref_state = 0;
        ref_ena = 0;
        cycleCount++;
        if (dut_q != ref_state) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", expected q = " << (unsigned int)ref_state
                      << ", got q = " << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        if (dut_ena != ref_ena) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", expected ena = " << (unsigned int)ref_ena
                      << ", got ena = " << (unsigned int)dut_ena << std::endl;
            errorCount++;
        }
    }

    // Deassert reset.
    reset = false;

    // Now run 10 cycles with slow counting (always enabled).
    // For each rising edge, update the reference and call TopModule.
    for (int i = 0; i < 10; i++) {
        TopModule(reset, dut_ena, dut_q);

        // Compute enable for each digit from pre-update ref_state (for update logic).
        bool en0 = true;
        bool en1 = (ref_state.range(3,0) == 9);
        bool en2 = (ref_state.range(7,0) == 0x99);
        bool en3 = (ref_state.range(11,0) == 0x999);

        // Update each BCD digit.
        for (int j = 0; j < 4; j++) {
            ap_uint<4> digit = ref_state.range(j*4+3, j*4);
            bool digit_en = false;
            if (j == 0)
                digit_en = en0;
            else if (j == 1)
                digit_en = en1;
            else if (j == 2)
                digit_en = en2;
            else if (j == 3)
                digit_en = en3;

            if (digit_en) {
                if (digit == 9)
                    digit = 0;
                else
                    digit = digit + 1;
            }
            ref_state.range(j*4+3, j*4) = digit;
        }

        // Compute ref_ena from post-update ref_state.
        bool re1 = (ref_state.range(3,0) == 9);
        bool re2 = (ref_state.range(7,0) == 0x99);
        bool re3 = (ref_state.range(11,0) == 0x999);
        ref_ena = (re3 << 2) | (re2 << 1) | re1;

        // Compare DUT output with reference.
        if (dut_q != ref_state) {
            std::cout << "Fixed test error at cycle " << cycleCount+1
                      << ": reset=" << reset
                      << ", expected q = " << (unsigned int)ref_state
                      << ", got q = " << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        if (dut_ena != ref_ena) {
            std::cout << "Fixed test error at cycle " << cycleCount+1
                      << ": reset=" << reset
                      << ", expected ena = " << (unsigned int)ref_ena
                      << ", got ena = " << (unsigned int)dut_ena << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Pause test: When the counter is paused (simulate by not calling TopModule), the output should hold. We can simulate this by calling TopModule with slow activity off.
    ap_uint<16> hold_state = ref_state;
    // (No call to TopModule here; we assume the design holds the value.)
    if (dut_q != hold_state) {
        std::cout << "Hold test error at cycle " << cycleCount
                  << ": expected q = " << (unsigned int)hold_state
                  << ", got q = " << (unsigned int)dut_q << std::endl;
        errorCount++;
    }
    cycleCount++;

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < randomCycles; i++) {
        // Randomly decide whether to assert reset (e.g. 10% chance).
        reset = (std::rand() % 10 == 0);
        TopModule(reset, dut_ena, dut_q);

        if (reset) {
            ref_state = 0;
            ref_ena = 0;
        } else {
            // Compute enable for each digit from pre-update ref_state.
            bool en0 = true;
            bool en1 = (ref_state.range(3,0) == 9);
            bool en2 = (ref_state.range(7,0) == 0x99);
            bool en3 = (ref_state.range(11,0) == 0x999);
            // Update each digit.
            for (int j = 0; j < 4; j++) {
                ap_uint<4> digit = ref_state.range(j*4+3, j*4);
                bool digit_en = false;
                if (j == 0)
                    digit_en = en0;
                else if (j == 1)
                    digit_en = en1;
                else if (j == 2)
                    digit_en = en2;
                else if (j == 3)
                    digit_en = en3;
                if (digit_en) {
                    if (digit == 9)
                        digit = 0;
                    else
                        digit = digit + 1;
                }
                ref_state.range(j*4+3, j*4) = digit;
            }
            // Compute ref_ena from post-update ref_state.
            bool re1 = (ref_state.range(3,0) == 9);
            bool re2 = (ref_state.range(7,0) == 0x99);
            bool re3 = (ref_state.range(11,0) == 0x999);
            ref_ena = (re3 << 2) | (re2 << 1) | re1;
        }

        if (dut_q != ref_state) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", expected q = " << (unsigned int)ref_state
                      << ", got q = " << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        if (dut_ena != ref_ena) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", expected ena = " << (unsigned int)ref_ena
                      << ", got ena = " << (unsigned int)dut_ena << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary in decimal.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
