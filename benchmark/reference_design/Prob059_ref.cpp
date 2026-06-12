#include "ap_int.h"

void TopModule(ap_uint<8> a, ap_uint<8> b, bool sel, ap_uint<8> &out) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=sel
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Correct mux behavior: if sel==1 then output equals a, else output equals b.
    out = sel ? a : b;
}