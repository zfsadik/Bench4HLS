#include <iostream>
#include "ap_int.h"

void TopModule(bool in, ap_uint<2> state, ap_uint<2> &next_state, bool &out);

int main() {
    int errorCount = 0;

    // Expected values from spec table: [state][in] -> {next_state, out}
    // State A=0, B=1, C=2, D=3
    ap_uint<2> exp_next[4][2] = {{0, 1}, {2, 1}, {0, 3}, {2, 1}};
    bool       exp_out[4]     = {0, 0, 0, 1};

    // Exhaustive enumeration of all 8 (state, in) combinations
    for (int s = 0; s < 4; s++) {
        for (int i = 0; i < 2; i++) {
            ap_uint<2> state = s;
            bool in = (bool)i;
            ap_uint<2> next_state_dut;
            bool out_dut;

            TopModule(in, state, next_state_dut, out_dut);

            if (next_state_dut != exp_next[s][i]) {
                std::cout << "Mismatch: state=" << s << " in=" << i
                          << ": expected next_state " << exp_next[s][i]
                          << ", got " << next_state_dut << std::endl;
                errorCount++;
            }
            if (out_dut != exp_out[s]) {
                std::cout << "Mismatch: state=" << s << " in=" << i
                          << ": expected out " << exp_out[s]
                          << ", got " << out_dut << std::endl;
                errorCount++;
            }
        }
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All 8 combinations matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected." << std::endl;
    }

    return 0;
}
