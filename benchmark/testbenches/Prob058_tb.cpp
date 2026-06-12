#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool w, bool R, bool E, bool L, bool &Q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 200;  // Total test cycles

    // Reference model for expected behavior.
    bool ref_state = 0;
    bool dut_Q = 0;

    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Simulation loop:
    for (int i = 0; i < totalCycles; i++) {
        bool w = (std::rand() % 2) ? true : false;
        bool R = (std::rand() % 2) ? true : false;
        bool E = (std::rand() % 2) ? true : false;
        bool L = (std::rand() % 2) ? true : false;

        TopModule(w, R, E, L, dut_Q);

        if (L)
            ref_state = R;
        else if (E)
            ref_state = w;

        cycleCount++;

        // Compare DUT output with reference expected state.
        if (dut_Q != ref_state) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": w = " << w
                      << ", R = " << R
                      << ", E = " << E
                      << ", L = " << L
                      << ", expected Q = " << ref_state
                      << ", got Q = " << dut_Q << std::endl;
            errorCount++;
        }
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
