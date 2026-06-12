#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool d, bool &q);

void ref_compute(bool d, bool &q_ref, bool &qp_ref) {
    q_ref = qp_ref;
    qp_ref = d;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Golden model variables.
    bool qp_ref = 0;
    bool golden_q = 0;

    // Variables for design output and input.
    bool d;
    bool dut_q = 0;

    // Total number of cycles to simulate.
    const int totalCycles = 110;

    // Simulate cycle steps.
    for (int i = 0; i < totalCycles; i++) {
        // For the first 10 cycles, generate d from a shifted random value.
        if (i < 10)
            d = ((std::rand() >> 2) & 1) ? true : false;
        else
            d = (std::rand() & 1) ? true : false;

        TopModule(d, dut_q);
        ref_compute(d, golden_q, qp_ref);

        // Compare design output to golden model.
        if (dut_q != golden_q) {
            std::cout << "Error at test case " << cycleCount
                      << ": input d = " << d
                      << " expected q = " << golden_q
                      << ", got q = " << dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
