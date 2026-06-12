#include "ap_int.h"

void TopModule(ap_uint<8> a, ap_uint<8> b, ap_uint<8> c, ap_uint<8> d, ap_uint<8> &min) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=min
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<8> min_ab = (a < b) ? a : b;
    ap_uint<8> min_cd = (c < d) ? c : d;
    min = (min_ab < min_cd) ? min_ab : min_cd;
}
