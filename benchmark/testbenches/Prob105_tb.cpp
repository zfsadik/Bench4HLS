#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool state, bool in, bool areset, bool &next_state, bool &out);

void ref_compute(bool state, bool in, bool areset, bool &next_state, bool &out) {
    if (areset) {
        next_state = true;
        out = true;
    } else {
        next_state = (state == in);  // XNOR from spec: B+1->B, B+0->A, A+0->B, A+1->A
        out = state;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool ref_state = false;      // Initial state (A) [output 0]
    bool in;
    bool areset;
    bool out_dut = false;
    bool out_ref = false;
    bool next_state_dut = false;
    bool next_state_ref = false;

    // Apply reset for 2 cycles.
    for (int i = 0; i < 2; i++) {
        in = false;
        TopModule(ref_state, in, true, next_state_dut, out_dut);
        ref_compute(ref_state, in, true, next_state_ref, out_ref);
        ref_state = next_state_ref;
    }

    // Simulation: run for 200 cycles.
    const int totalCycles = 200;
    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus: in is random; areset is randomly asserted (25% chance).
        in = (std::rand() & 1) ? true : false;
        areset = ((std::rand() % 4) == 0);  // Approximately 25% probability.

        TopModule(ref_state, in, areset, next_state_dut, out_dut);
        ref_compute(ref_state, in, areset, next_state_ref, out_ref);

        if (out_dut != out_ref || next_state_dut != next_state_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out=" << out_ref << " next_state=" << next_state_ref
                      << ", got out=" << out_dut << " next_state=" << next_state_dut << std::endl;
            errorCount++;
        }
        ref_state = next_state_ref;
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
