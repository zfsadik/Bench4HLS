#include <iostream>
#include <cstdlib>

// DUT prototype
void TopModule(
    bool a,
    bool b,
    bool &q,
    bool &state
);

// Inline reference model
void RefModule(
    bool a,
    bool b,
    bool &q_ref,
    bool &state_ref,
    bool &c_ref
) {
    bool c_next = (a && b) || (a && c_ref) || (b && c_ref);
    q_ref     = a ^ b ^ c_ref;
    state_ref = c_ref;
    c_ref     = c_next;
}

int main() {
    const int FIXED_CYCLES = 4;
    const int RANDOM_CYCLES = 1000;
    bool a = 0, b = 0;
    bool q_dut = 0, state_dut = 0;
    bool q_ref = 0, state_ref = 0;
    bool c_ref = 0;
    int cycleCount = 0, errorCount = 0;

    // Fixed test: cycle through (a,b) = (0,0),(0,1),(1,0),(1,1)
    bool fixed[FIXED_CYCLES][2] = {
        {0,0}, {0,1}, {1,0}, {1,1}
    };

    // Run fixed cycles first
    for (int i = 0; i < FIXED_CYCLES; ++i) {
        a = fixed[i][0];
        b = fixed[i][1];
        TopModule(a, b, q_dut, state_dut);
        RefModule(a, b, q_ref, state_ref, c_ref);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected q=" << q_ref
                      << ", got q="      << q_dut << std::endl;
            errorCount++;
        }
        if (state_dut != state_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected state=" << state_ref
                      << ", got state="      << state_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Random stimulus
    std::srand(0);
    for (int i = 0; i < RANDOM_CYCLES; ++i) {
        a = (std::rand() & 1);
        b = (std::rand() & 1);
        TopModule(a, b, q_dut, state_dut);
        RefModule(a, b, q_ref, state_ref, c_ref);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected q=" << q_ref
                      << ", got q="      << q_dut << std::endl;
            errorCount++;
        }
        if (state_dut != state_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected state=" << state_ref
                      << ", got state="      << state_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report summary
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
