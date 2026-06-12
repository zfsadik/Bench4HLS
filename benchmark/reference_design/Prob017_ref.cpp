#include "ap_int.h"

void TopModule(ap_uint<100> a, ap_uint<100> b, bool sel, ap_uint<100> &out) {

    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=sel
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = sel ? b : a;
}
