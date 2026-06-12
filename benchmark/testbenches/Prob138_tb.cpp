#include <iostream>
#include <cstdlib>
#include "ap_int.h"

// DUT
void TopModule(
    bool        in,
    ap_uint<10> state,
    ap_uint<10> &next_state,
    bool       &out1,
    bool       &out2
);

// Reference model: table-driven from state machine spec
void ref_model(
    bool        in,
    ap_uint<10> state,
    ap_uint<10> &next_state,
    bool       &out1,
    bool       &out2
) {
    // outputs: S7 -> out2=1, S8 -> out1=1, S9 -> out1=1 out2=1
    out1 = state[8] || state[9];
    out2 = state[7] || state[9];

    next_state = 0;
    // S0: --0-->S0, --1-->S1
    if (state[0]) next_state[in ? 1 : 0] = 1;
    // S1: --0-->S0, --1-->S2
    if (state[1]) next_state[in ? 2 : 0] = 1;
    // S2: --0-->S0, --1-->S3
    if (state[2]) next_state[in ? 3 : 0] = 1;
    // S3: --0-->S0, --1-->S4
    if (state[3]) next_state[in ? 4 : 0] = 1;
    // S4: --0-->S0, --1-->S5
    if (state[4]) next_state[in ? 5 : 0] = 1;
    // S5: --0-->S8, --1-->S6
    if (state[5]) next_state[in ? 6 : 8] = 1;
    // S6: --0-->S9, --1-->S7
    if (state[6]) next_state[in ? 7 : 9] = 1;
    // S7: --0-->S0, --1-->S7
    if (state[7]) next_state[in ? 7 : 0] = 1;
    // S8: --0-->S0, --1-->S1
    if (state[8]) next_state[in ? 1 : 0] = 1;
    // S9: --0-->S0, --1-->S1
    if (state[9]) next_state[in ? 1 : 0] = 1;
}

int main() {
    int errorCount = 0, cycleCount = 0;
    std::srand(123);

    for (int t = 0; t < 500; ++t) {
        bool        in        = std::rand() & 1;
        ap_uint<10> state     = 1 << (std::rand() % 10);
        ap_uint<10> next_dut, next_ref;
        bool        out1_dut, out2_dut;
        bool        out1_ref, out2_ref;

        TopModule(in, state, next_dut, out1_dut, out2_dut);
        ref_model(in, state, next_ref, out1_ref, out2_ref);

        for (int i = 0; i < 10; ++i) {
            if (next_dut[i] != next_ref[i]) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": next_state[" << i << "] expected "
                          << next_ref[i] << ", got " << next_dut[i]
                          << std::endl;
                errorCount++;
            }
        }
        if (out1_dut != out1_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": out1 expected " << out1_ref
                      << ", got " << out1_dut << std::endl;
            errorCount++;
        }
        if (out2_dut != out2_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": out2 expected " << out2_ref
                      << ", got " << out2_dut << std::endl;
            errorCount++;
        }

        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." 
                  << std::endl;
    }
    return 0;
}
