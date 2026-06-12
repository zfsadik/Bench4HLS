#include "ap_int.h"

void TopModule(ap_uint<4> a, ap_uint<4> b, ap_uint<4> c, ap_uint<4> d, ap_uint<4> e, ap_uint<4> & q) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=e
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return

    switch(c) {
        case 0: q = b; break;
        case 1: q = e; break;
        case 2: q = a; break;
        case 3: q = d; break;
        default: q = 0xF; break;
    }
}
