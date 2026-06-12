#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool in, bool reset, bool &done);

ap_uint<4> ref_update(ap_uint<4> st, bool reset, bool in) {
    if (reset) return 8; // START
    switch (st) {
      case 8:  return in ? 8 : 0;   // START→B0
      case 0:  return 1;            // B0→B1
      case 1:  return 2;            // B1→B2
      case 2:  return 3;            // B2→B3
      case 3:  return 4;            // B3→B4
      case 4:  return 5;            // B4→B5
      case 5:  return 6;            // B5→B6
      case 6:  return 7;            // B6→B7
      case 7:  return 9;            // B7→STOP
      case 9:  return in ? 10 : 11; // STOP→DONE or ERR
      case 10: return 8;             // DONE→START
      case 11: return in ? 8 : 11;  // ERR→START or stay ERR
      default: return 8;
    }
}

// Reference output
bool ref_out(ap_uint<4> st) {
    return (st == 10);
}

// Simulate one clock cycle
void cycle(bool reset, bool in,
           bool &done_dut,
           ap_uint<4> &ref_st,
           int &cycleCount, int &errorCount) {
    bool done_ref = ref_out(ref_st);
    TopModule(in, reset, done_dut);
    ref_st = ref_update(ref_st, reset, in);
    if (done_dut != done_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected " << done_ref
                  << ", got " << done_dut << std::endl;
        errorCount++;
    }
    cycleCount++;
}

int main() {
    int errorCount = 0, cycleCount = 0;
    bool in, reset, done_dut;
    ap_uint<4> ref_st = 8; // START

    // Stimulus per Verilog testbench:
    // reset=1, in=1 for two cycles
    reset = true; in = true;
    cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // reset=0, start a correct byte: start bit(0) + 8 data + stop(1)
    reset = false;
    in = false;
    for (int i = 0; i < 9; i++) cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    in = true; cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // Repeat for several bytes, introducing an error case:
    in = false;
    for (int i = 0; i < 9; i++) cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    in = true; cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // Introduce a missing stop (in=0 for stop bit)
    in = false;
    for (int i = 0; i < 10; i++) cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    in = true; cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // Another good byte
    in = false;
    for (int i = 0; i < 10; i++) cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    in = true; cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // And one more
    in = false;
    for (int i = 0; i < 9; i++) cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
    in = true; cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);

    // Randomized testing (800 cycles)
    for (int i = 0; i < 800; i++) {
        in    = rand() & 1;
        reset = ((rand() & 31) == 0);
        cycle(reset, in, done_dut, ref_st, cycleCount, errorCount);
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
