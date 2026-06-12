#include "ap_int.h"

void TopModule(bool resetn, bool in, bool &out) {
    #pragma HLS INTERFACE ap_none port=resetn
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<4> sr = 0;

    // Synchronous active-low reset: if resetn is low, clear the register.
    if (!resetn)
        sr = 0;
    else
        sr = (sr << 1) | in;

    out = sr[3];
}
