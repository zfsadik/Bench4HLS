#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool a, bool &q);

void ref_compute(bool a, bool &q_ref, bool &reg_q_ref) {
    q_ref = reg_q_ref;
    reg_q_ref = !a;
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool a = false;
    bool q_dut = false;  // Output from DUT.
    bool q_ref = false;  // Expected output.
    bool reg_q_ref = false;

    // Run simulation for a fixed number of cycles.
    const int totalCycles = 100;
    for (int i = 0; i < totalCycles; i++) {
        // Randomly generate input 'a' (0 or 1).
        a = ((std::rand() & 1) != 0);

        TopModule(a, q_dut);
        ref_compute(a, q_ref, reg_q_ref);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
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
