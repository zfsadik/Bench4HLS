#include <iostream>
#include <cstdlib>
#include <ap_int.h>

// DUT prototype
void TopModule(
    bool         resetn,
    ap_uint<3>   r,
    ap_uint<3> & g
);

enum States { A=0, B=1, C=2, D=3 };

// Inline reference model
void RefModule(
    bool         resetn,
    ap_uint<3>   r,
    ap_uint<3> & g_ref,
    States     & state_ref
) {
    g_ref = 0;
    if (state_ref == B) g_ref[0] = 1;
    if (state_ref == C) g_ref[1] = 1;
    if (state_ref == D) g_ref[2] = 1;

    if (!resetn) {
        state_ref = A;
    } else {
        switch (state_ref) {
          case A:
            if (r[0])      state_ref = B;
            else if (r[1]) state_ref = C;
            else if (r[2]) state_ref = D;
            else           state_ref = A;
            break;
          case B:
            state_ref = r[0] ? B : A;
            break;
          case C:
            state_ref = r[1] ? C : A;
            break;
          case D:
            state_ref = r[2] ? D : A;
            break;
        }
    }
}

int main() {
    const int TOTAL_CYCLES = 1000;
    bool resetn;
    ap_uint<3> r, g_dut, g_ref;
    States state_ref = A;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    // Reset for 2 cycles
    for (int i = 0; i < 2; i++) {
        TopModule(false, 0, g_dut);
        RefModule(false, 0, g_ref, state_ref);
    }

    for (int i = 0; i < TOTAL_CYCLES; ++i) {
        resetn = ((std::rand() & 7) != 0);

        r[0] = (std::rand() & 1);
        r[1] = (std::rand() & 1);
        r[2] = (std::rand() & 1);

        TopModule(resetn, r, g_dut);
        RefModule(resetn, r, g_ref, state_ref);

        if (g_dut != g_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected g=" << g_ref
                      << ", got g="      << g_dut << std::endl;
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
