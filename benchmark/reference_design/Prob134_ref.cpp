#include "ap_int.h"

void TopModule(bool resetn, bool x, bool y, bool &f, bool &g) {
    #pragma HLS INTERFACE ap_none port=resetn
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=f
    #pragma HLS INTERFACE ap_none port=g
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State codes: A=0, B=1, S0=2, S1=3, S10=4, G1=5, G2=6, P0=7, P1=8
    static ap_uint<4> state = 0;
    #pragma HLS RESET variable=state
    ap_uint<4> next;

    f = (state == 1);
    g = (state == 5) || (state == 6) || (state == 8);

    if (!resetn) {
        state = 0;
    } else {
        // Combinational next‑state logic via if/else
        if      (state == 0) next = 1;              // A → B
        else if (state == 1) next = 2;              // B → S0
        else if (state == 2) next = x ? 3 : 2;      // S0 → S1/S0
        else if (state == 3) next = x ? 3 : 4;      // S1 → S1/S10
        else if (state == 4) next = x ? 5 : 2;      // S10 → G1/S0
        else if (state == 5) next = y ? 8 : 6;      // G1 → P1/G2
        else if (state == 6) next = y ? 8 : 7;      // G2 → P1/P0
        else if (state == 7) next = 7;              // P0 → P0
        else if (state == 8) next = 8;              // P1 → P1
        else                  next = 0;             // default → A
        state = next;
    }
}
