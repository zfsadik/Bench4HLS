#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool x, bool y, bool &z);

bool ref_model(bool x, bool y) {
    return x | (!y);
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool x, y, z_dut, z_ref;

    // Exhaustive test of all 4 input combinations
    for (int xi = 0; xi < 2; xi++) {
        for (int yi = 0; yi < 2; yi++) {
            x = xi;
            y = yi;
            TopModule(x, y, z_dut);
            z_ref = ref_model(x, y);

            if (z_dut != z_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected " << z_ref
                          << ", got " << z_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Randomized testing (100 cases)
    for (int i = 0; i < 100; i++) {
        x = rand() & 1;
        y = rand() & 1;
        TopModule(x, y, z_dut);
        z_ref = ref_model(x, y);

        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Test summary
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
