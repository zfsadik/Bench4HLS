#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool resetn, bool x, bool y, bool &f, bool &g);

ap_uint<4> ref_update(ap_uint<4> st, bool resetn, bool x, bool y) {
    // State codes: A=0, B=1, S0=2, S1=3, S10=4, G1=5, G2=6, P0=7, P1=8
	if (!resetn) return 0; // A
    switch (st) {
        case 0:  return 1;              // A→B
        case 1:  return 2;              // B→S0
        case 2:  return x ? 3 : 2;      // S0→S1/S0
        case 3:  return x ? 3 : 4;      // S1→S1/S10
        case 4:  return x ? 5 : 2;      // S10→G1/S0
        case 5:  return y ? 8 : 6;      // G1→P1/G2
        case 6:  return y ? 8 : 7;      // G2→P1/P0
        case 7:  return 7;              // P0
        case 8:  return 8;              // P1
        default: return 0;
    }
}

// Reference outputs
bool ref_f(ap_uint<4> st) { return st == 1; }
bool ref_g(ap_uint<4> st) { return (st==5)||(st==6)||(st==8); }

// Simulate one clock cycle
void cycle(bool resetn, bool x, bool y,
           bool &f_dut, bool &g_dut,
           ap_uint<4> &ref_st,
           int &cycleCount, int &errorCount) {
    TopModule(resetn, x, y, f_dut, g_dut);
    bool f_ref = ref_f(ref_st);
    bool g_ref = ref_g(ref_st);
    ref_st = ref_update(ref_st, resetn, x, y);
    if (f_dut != f_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected f="<<f_ref
                  << ", got "<<f_dut<<"\n";
        errorCount++;
    }
    if (g_dut != g_ref) {
        std::cout << "Mismatch at cycle " << cycleCount
                  << ": expected g="<<g_ref
                  << ", got "<<g_dut<<"\n";
        errorCount++;
    }
    cycleCount++;
}

int main(){
    int errorCount=0, cycleCount=0;
    bool resetn, x, y, f_dut, g_dut;
    ap_uint<4> ref_st = 0;

    // Initial reset pulse (≥2 cycles)
    resetn = false; x = 0; y = 0;
    cycle(resetn,x,y,f_dut,g_dut,ref_st,cycleCount,errorCount);
    cycle(resetn,x,y,f_dut,g_dut,ref_st,cycleCount,errorCount);
    resetn = true;

    // Random stimulus: 500 cycles
    for(int i=0;i<500;i++){
        x     = rand()&1;
        y     = rand()&1;
        resetn= ((rand()&31)!=0);
        cycle(resetn,x,y,f_dut,g_dut,ref_st,cycleCount,errorCount);
    }

    // Test summary
    if(errorCount==0)
        std::cout<<"Test Passed: All "<<cycleCount<<" test cases matched.\n";
    else
        std::cout<<"Test Failed: "<<errorCount
                 <<" mismatches detected out of "<<cycleCount<<" cases.\n";
    return 0;
}
