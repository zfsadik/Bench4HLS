#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, ap_uint<1> &out, ap_uint<1> &out_n) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_none port=out_n
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<1> and1 = a & b;
    ap_uint<1> and2 = c & d;

    out = and1 | and2;

    out_n = ~out;
}
