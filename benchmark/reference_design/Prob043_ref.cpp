#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8> &anyedge) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=anyedge
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<8> d_last = 0;

    anyedge = in ^ d_last;
    d_last = in;
}
