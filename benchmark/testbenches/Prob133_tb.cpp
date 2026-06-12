#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool reset, bool w, bool &z);

ap_uint<3> ref_update(ap_uint<3> st, bool reset, bool w) {
    if (reset) return 0;
    switch (st) {
        case 0: return w ? 1 : 0;
        case 1: return w ? 2 : 3;
        case 2: return w ? 4 : 3;
        case 3: return w ? 5 : 0;
        case 4: return w ? 4 : 3;
        case 5: return w ? 2 : 3;
        default:return 0;
    }
}

// Reference output
bool ref_out(ap_uint<3> st) {
    return (st == 4) || (st == 5);
}

// Simulate one cycle
void cycle(bool reset, bool w,
           bool &z_dut,
           ap_uint<3> &ref_st,
           int &cycleCount, int &errorCount) {
    bool z_ref = ref_out(ref_st);
    TopModule(reset, w, z_dut);
    ref_st = ref_update(ref_st, reset, w);
    if (z_dut != z_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << z_ref
                  << ", got " << z_dut << std::endl;
        errorCount++;
    }
    cycleCount++;
}

int main() {
    int errorCount = 0, cycleCount = 0;
    bool reset, w, z_dut;
    ap_uint<3> ref_st = 0;

    // Reset for 2 cycles
    for (int i = 0; i < 2; i++) {
        cycle(true, false, z_dut, ref_st, cycleCount, errorCount);
    }

    // Run 200 cycles with random w/reset
    for (int i = 0; i < 200; i++) {
        w     = rand() & 1;
        reset = ((rand() & 31) == 0);
        cycle(reset, w, z_dut, ref_st, cycleCount, errorCount);
    }

    // Summary
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
