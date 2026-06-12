#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool j, bool k, bool &Q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100;  // Total number of test cycles

    // Reference model to simulate the expected behavior of the JK flip-flop.
    bool ref_state = 0;
    bool dut_Q = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulation loop: each iteration invokes the DUT once (one combinational evaluation).
    for (int i = 0; i < totalCycles; i++) {
        // Generate random values for j and k.
        bool j = (std::rand() % 2) ? true : false;
        bool k = (std::rand() % 2) ? true : false;

        TopModule(j, k, dut_Q);

        if (!j && k) {
            ref_state = false;
        } else if (j && !k) {
            ref_state = true;
        } else if (j && k) {
            ref_state = !ref_state;
        }
        cycleCount++;

        // Compare DUT output with the expected reference state.
        if (dut_Q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": j = " << j
                      << ", k = " << k
                      << ", expected Q = " << ref_state
                      << ", got Q = " << dut_Q << std::endl;
            errorCount++;
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
