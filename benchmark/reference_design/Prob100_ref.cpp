#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, ap_uint<1>& q) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return

    q = (((~a) ^ b) ^ c) ^ d;
}
