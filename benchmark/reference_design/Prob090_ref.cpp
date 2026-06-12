#include "ap_int.h"

void TopModule(bool c, bool d, ap_uint<4> &mux_in) {
#pragma HLS INTERFACE ap_none port=c
#pragma HLS INTERFACE ap_none port=d
#pragma HLS INTERFACE ap_none port=mux_in
#pragma HLS INTERFACE ap_ctrl_none port=return

    bool bit0 = c || d;
    bool bit1 = false;
    bool bit2 = c && d;
    bool bit3 = c || !d;

    mux_in[0] = bit0;
    mux_in[1] = bit1;
    mux_in[2] = bit2;
    mux_in[3] = bit3;
}
