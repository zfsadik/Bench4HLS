#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool in, ap_uint<4> state, ap_uint<4> &next_state, bool &out);

// Golden model derived directly from state transition table (one-hot: A=0001, B=0010, C=0100, D=1000)
static void golden(bool in, ap_uint<4> state, ap_uint<4> &next_state, bool &out) {
    switch ((int)state) {
        case 0x1: next_state = in ? 0x2 : 0x1; out = 0; break; // A: in=0->A, in=1->B, out=0
        case 0x2: next_state = in ? 0x2 : 0x4; out = 0; break; // B: in=0->C, in=1->B, out=0
        case 0x4: next_state = in ? 0x8 : 0x1; out = 0; break; // C: in=0->A, in=1->D, out=0
        case 0x8: next_state = in ? 0x2 : 0x4; out = 1; break; // D: in=0->C, in=1->B, out=1
        default:  next_state = 0; out = 0; break;
    }
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    std::srand(static_cast<unsigned int>(std::time(0)));

    bool in;
    ap_uint<4> state;
    ap_uint<4> dut_next_state;
    bool dut_out;

    ap_uint<4> expected_next_state;
    bool expected_out;

    // --- Exhaustive test using one-hot state values ---
    for (int i = 0; i < 4; i++) {
        state = 1 << i;
        for (int j = 0; j < 2; j++) {
            in = (j == 1);
            TopModule(in, state, dut_next_state, dut_out);
            golden(in, state, expected_next_state, expected_out);

            if (dut_next_state != expected_next_state) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (in, state) = (" << in << ", " << state << ") "
                          << "expected next_state = " << expected_next_state
                          << ", got next_state = " << dut_next_state << std::endl;
                errorCount++;
            }
            if (dut_out != expected_out) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (in, state) = (" << in << ", " << state << ") "
                          << "expected out = " << expected_out
                          << ", got out = " << dut_out << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // --- Random test cases (valid one-hot states only) ---
    for (int i = 0; i < 200; i++) {
        state = 1 << (std::rand() % 4);
        in = (std::rand() % 2) ? true : false;

        TopModule(in, state, dut_next_state, dut_out);
        golden(in, state, expected_next_state, expected_out);

        if (dut_next_state != expected_next_state) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (in, state) = (" << in << ", " << state << ") "
                      << "expected next_state = " << expected_next_state
                      << ", got next_state = " << dut_next_state << std::endl;
            errorCount++;
        }
        if (dut_out != expected_out) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (in, state) = (" << in << ", " << state << ") "
                      << "expected out = " << expected_out
                      << ", got out = " << dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
