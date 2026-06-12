#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool reset, bool s, bool w, bool &z);

ap_uint<3> ref_update(ap_uint<3> state, bool reset, bool s, bool w) {
    if (reset) return 0; // A
    switch (state) {
      case 0: return s ? 1 : 0;
      case 1: return w ? 3 : 2;
      case 2: return w ? 5 : 4;
      case 3: return w ? 6 : 5;
      case 4: return 1;
      case 5: return w ? 7 : 1;
      case 6: return w ? 1 : 7;
      case 7: return 1;
      default: return 0;
    }
}

bool ref_out(ap_uint<3> state) {
    return state == 7;
}

void simulateCycle(bool reset, bool s, bool w,
                   bool &z_dut,
                   ap_uint<3> &ref_state,
                   int &cycleCount,
                   int &errorCount) {
    TopModule(reset, s, w, z_dut);
    bool z_ref = ref_out(ref_state);
    ref_state = ref_update(ref_state, reset, s, w);
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
    bool reset, s, w, z_dut;
    ap_uint<3> ref_state = 0;

    // Phase 1: Reset pulse, idle in A
    reset = true; s = false; w = false;
    for (int i = 0; i < 2; i++)
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);

    // Deassert reset, 2 cycles idle in A
    reset = false;
    for (int i = 0; i < 2; i++)
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);

    // s==1: enter B, now test 200 random w's to exercise sliding windows
    s = true;
    for (int i = 0; i < 200; i++) {
        w = rand() & 1;
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);
    }

    // Phase 2: Another reset pulse
    reset = true;
    simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);
    reset = false;
    for (int i = 0; i < 2; i++)
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);

    // 200 more random w's with s==1
    for (int i = 0; i < 200; i++) {
        w = rand() & 1;
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);
    }

    // Phase 3: 500 fully random cycles (reset, s, w all vary)
    for (int i = 0; i < 500; i++) {
        reset = (rand() & 31) == 0;
        s     = rand() & 1;
        w     = rand() & 1;
        simulateCycle(reset, s, w, z_dut, ref_state, cycleCount, errorCount);
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
