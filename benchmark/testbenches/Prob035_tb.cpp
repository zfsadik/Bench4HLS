#include <iostream>
#include "ap_int.h"

void TopModule(bool ring, bool vibrate_mode, bool &ringer, bool &motor);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int r = 0; r <= 1; r++) {
        for (int v = 0; v <= 1; v++) {
            bool ring = (bool)r;
            bool vibrate_mode = (bool)v;
            bool dut_ringer = false;
            bool dut_motor = false;

            TopModule(ring, vibrate_mode, dut_ringer, dut_motor);

            bool expected_ringer = ring && (!vibrate_mode);
            bool expected_motor = ring && vibrate_mode;

            // Check for mismatches.
            if (dut_ringer != expected_ringer) {
                std::cout << "Mismatch in ringer at cycle " << cycleCount
                          << ": ring = " << ring
                          << ", vibrate_mode = " << vibrate_mode
                          << ", expected ringer = " << expected_ringer
                          << ", got ringer = " << dut_ringer << std::endl;
                errorCount++;
            }
            if (dut_motor != expected_motor) {
                std::cout << "Mismatch in motor at cycle " << cycleCount
                          << ": ring = " << ring
                          << ", vibrate_mode = " << vibrate_mode
                          << ", expected motor = " << expected_motor
                          << ", got motor = " << dut_motor << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
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
