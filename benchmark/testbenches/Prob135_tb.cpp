#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool reset, bool in, bool &disc, bool &flag, bool &err);

ap_uint<4> ref_update(ap_uint<4> st, bool reset, bool in) {
    if (reset) return 0;
    if      (st == 0) return in ? 1 : 0;
    else if (st == 1) return in ? 2 : 0;
    else if (st == 2) return in ? 3 : 0;
    else if (st == 3) return in ? 4 : 0;
    else if (st == 4) return in ? 5 : 0;
    else if (st == 5) return in ? 6 : 8;
    else if (st == 6) return in ? 7 : 9;
    else if (st == 7) return in ? 7 : 0;
    else if (st == 9) return in ? 1 : 0;
    else if (st == 8) return in ? 1 : 0;
    else              return 0;
}

// Reference outputs
bool ref_disc(ap_uint<4> st) { return st == 8; }
bool ref_flag(ap_uint<4> st) { return st == 9; }
bool ref_err (ap_uint<4> st) { return st == 7; }

// Simulate one clock cycle
void cycle(bool reset, bool in,
           bool &disc_dut, bool &flag_dut, bool &err_dut,
           ap_uint<4> &ref_st,
           int &cycleCount, int &errorCount) {
    // Moore outputs from pre-update (registered) state
    bool d_ref = ref_disc(ref_st),
         f_ref = ref_flag(ref_st),
         e_ref = ref_err(ref_st);

    TopModule(reset, in, disc_dut, flag_dut, err_dut);
    ref_st = ref_update(ref_st, reset, in);

    if (disc_dut != d_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected disc=" << d_ref
                  << ", got " << disc_dut << "\n";
        errorCount++;
    }
    if (flag_dut != f_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected flag=" << f_ref
                  << ", got " << flag_dut << "\n";
        errorCount++;
    }
    if (err_dut != e_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected err=" << e_ref
                  << ", got " << err_dut << "\n";
        errorCount++;
    }
    cycleCount++;
}

int main() {
    bool reset, in, disc_dut, flag_dut, err_dut;
    ap_uint<4> ref_st = 0;
    int cycleCount = 0, errorCount = 0;

    // Apply reset for 2 cycles before normal stimulus
    reset = true; in = false;
    cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);
    cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);

    // Directed test: disc sequence (0 11111 0 0) — expect disc=1 on last cycle
    reset = false;
    { bool seq[] = {false, true, true, true, true, true, false, false};
      for (int i = 0; i < 8; i++) { in = seq[i]; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount); }
      if (!disc_dut) { std::cout << "Directed FAIL: disc not asserted after disc sequence\n"; errorCount++; } }

    // Directed test: flag sequence (reset, 0 111111 0 0) — expect flag=1 on last cycle
    reset = true; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);
    reset = false;
    { bool seq[] = {false, true, true, true, true, true, true, false, false};
      for (int i = 0; i < 9; i++) { in = seq[i]; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount); }
      if (!flag_dut) { std::cout << "Directed FAIL: flag not asserted after flag sequence\n"; errorCount++; } }

    // Directed test: err sequence (reset, 0 1111111 0) — expect err=1 on last cycle
    reset = true; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);
    reset = false;
    { bool seq[] = {false, true, true, true, true, true, true, true, false};
      for (int i = 0; i < 9; i++) { in = seq[i]; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount); }
      if (!err_dut) { std::cout << "Directed FAIL: err not asserted after err sequence\n"; errorCount++; } }

    // 800 random stimulus cycles
    reset = true; cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);
    for (int i = 0; i < 800; i++) {
        reset = ((rand() & 31) == 0);
        in    = ((rand() & 7) != 0);
        cycle(reset, in, disc_dut, flag_dut, err_dut, ref_st, cycleCount, errorCount);
    }

    // Final summary
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
