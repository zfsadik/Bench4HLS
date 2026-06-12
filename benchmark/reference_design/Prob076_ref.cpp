#include "ap_int.h"

void TopModule(bool in, ap_uint<4> state, ap_uint<4> &next_state, bool &out) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_none port=next_state
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    bool sA = state[0]; // A
    bool sB = state[1]; // B
    bool sC = state[2]; // C
    bool sD = state[3]; // D

    bool nsA = (sA || sC) && (!in);
    bool nsB = (sA || sB || sD) && in;
    bool nsC = (sB || sD) && (!in);
    bool nsD = sC && in;

    next_state[0] = nsA;
    next_state[1] = nsB;
    next_state[2] = nsC;
    next_state[3] = nsD;

    out = sD;
}
