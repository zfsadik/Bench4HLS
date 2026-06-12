#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool d, bool &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 110;  // Total test cycles

	// Reference model for the D flip-flop.
    bool ref_state = 0;       // Initial state of the reference DFF.

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Test loop: one call per cycle (one rising edge per iteration).
    for (int i = 0; i < totalCycles; i++) {
        bool d = (std::rand() % 2) ? true : false;
        bool q_dut = false;

        TopModule(d, q_dut);

        // Compare the DUT output with the reference state.
        if (q_dut != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": d = " << d
                      << ", expected q = " << ref_state
                      << ", got q = " << q_dut << std::endl;
            errorCount++;
        }

        // Reference model: capture d on each rising edge.
        ref_state = d;
        cycleCount++;
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
