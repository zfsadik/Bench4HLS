#include "ap_fixed.h"

void TopModule(ap_fixed<8,1> sample_in, ap_fixed<19,4> &sample_out) {
#pragma HLS INTERFACE ap_none port=sample_in
#pragma HLS INTERFACE ap_none port=sample_out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    const ap_fixed<16,1> h[2] = { 0.30011, 0.90032 };

    static ap_fixed<8,1> regs[4] = { 0, 0, 0, 0 };
#pragma HLS ARRAY_PARTITION variable=regs complete

    // shift right and insert new sample at tap 0
    regs[3] = regs[2];
    regs[2] = regs[1];
    regs[1] = regs[0];
    regs[0] = sample_in;

    ap_fixed<9,2> sym0 = regs[0] + regs[3];
    ap_fixed<9,2> sym1 = regs[1] + regs[2];
    sample_out = h[0] * sym0 + h[1] * sym1;
}
