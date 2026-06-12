#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool slowena, bool reset, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int numFixedCycles = 20;    // Number of fixed test cycles
    const int numRandomCycles = 100;    // Number of random test cycles

    // Reference model variables.
    ap_uint<4> ref_q = 0;   // Expected counter value

    // DUT output.
    ap_uint<4> dut_q = 0;

    // Test scenario: Fixed sequence
    {
        // Assert reset on the first rising edge.
        bool slowena = true;
        bool reset = true;
        TopModule(slowena, reset, dut_q);
        // On a rising edge with reset asserted, the reference model resets.
        ref_q = 0;
        if (dut_q != ref_q) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", slowena=" << slowena
                      << ", expected q=" << (unsigned int)ref_q
                      << ", got q=" << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
        TopModule(slowena, reset, dut_q);
        ref_q = 0;
        if (dut_q != ref_q) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", slowena=" << slowena
                      << ", expected q=" << (unsigned int)ref_q
                      << ", got q=" << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;

        // Deassert reset and then enable counting.
        reset = false;
        // Now, run through 10 cycles with slowena asserted.
        for (int i = 0; i < 10; i++) {
            TopModule(slowena, reset, dut_q);

            // Update reference: if slowena is true, increment (wrapping at 9).
            if (ref_q >= 9)
                ref_q = 0;
            else
                ref_q = ref_q + 1;

            if (dut_q != ref_q) {
                std::cout << "Fixed test error at cycle " << cycleCount
                          << ": reset=" << reset
                          << ", slowena=" << slowena
                          << ", expected q=" << (unsigned int)ref_q
                          << ", got q=" << (unsigned int)dut_q << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Now, perform a fixed pause test: With slowena deasserted, counter should hold.
    {
        bool slowena = false;
        bool reset = false;
        // Simulate one cycle with slowena false.
        TopModule(slowena, reset, dut_q);
        // The reference model holds its value.
        // Compare DUT output with ref_q.
        if (dut_q != ref_q) {
            std::cout << "Fixed hold test error at cycle " << cycleCount
                      << ": slowena=" << slowena
                      << ", expected q=" << (unsigned int)ref_q
                      << ", got q=" << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // For each cycle, randomly generate slowena and reset signals.
    for (int i = 0; i < numRandomCycles; i++) {
        bool slowena = (std::rand() % 2) ? true : false;
        bool reset = (std::rand() % 8 == 0);  // About 12.5% chance to assert reset

        TopModule(slowena, reset, dut_q);

        // Update the reference model.
        if (reset)
            ref_q = 0;
        else if (slowena) {
            if (ref_q >= 9)
                ref_q = 0;
            else
                ref_q = ref_q + 1;
        }

        if (dut_q != ref_q) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": reset=" << reset
                      << ", slowena=" << slowena
                      << ", expected q=" << (unsigned int)ref_q
                      << ", got q=" << (unsigned int)dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
