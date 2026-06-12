#include "ap_int.h"

void TopModule(ap_uint<16> in, ap_uint<8> &out_hi, ap_uint<8> &out_lo) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out_hi
    #pragma HLS INTERFACE ap_none port=out_lo
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out_hi = in.range(15, 8);  // Upper 8 bits.
    out_lo = in.range(7, 0);   // Lower 8 bits.
}
