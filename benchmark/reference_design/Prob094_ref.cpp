#include "ap_int.h"

void TopModule(ap_uint<16> a,
               ap_uint<16> b,
               ap_uint<16> c,
               ap_uint<16> d,
               ap_uint<16> e,
               ap_uint<16> f,
               ap_uint<16> g,
               ap_uint<16> h,
               ap_uint<16> i,
               ap_uint<4> sel,
               ap_uint<16> &out) {
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_none port=c
#pragma HLS INTERFACE ap_none port=d
#pragma HLS INTERFACE ap_none port=e
#pragma HLS INTERFACE ap_none port=f
#pragma HLS INTERFACE ap_none port=g
#pragma HLS INTERFACE ap_none port=h
#pragma HLS INTERFACE ap_none port=i
#pragma HLS INTERFACE ap_none port=sel
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

    switch (sel) {
        case 0: out = a; break;
        case 1: out = b; break;
        case 2: out = c; break;
        case 3: out = d; break;
        case 4: out = e; break;
        case 5: out = f; break;
        case 6: out = g; break;
        case 7: out = h; break;
        case 8: out = i; break;
        default: out = 0xFFFF; break;
    }
}
