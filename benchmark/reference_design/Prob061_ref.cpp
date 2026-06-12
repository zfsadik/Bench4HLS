#include "ap_int.h"

void TopModule(ap_uint<5> a, ap_uint<5> b, ap_uint<5> c, 
               ap_uint<5> d, ap_uint<5> e, ap_uint<5> f,
               ap_uint<8> &w, ap_uint<8> &x, ap_uint<8> &y, ap_uint<8> &z) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=e
    #pragma HLS INTERFACE ap_none port=f
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<32> concat;
    concat.range(31,27) = a; concat.range(26,22) = b;
    concat.range(21,17) = c; concat.range(16,12) = d;
    concat.range(11,7)  = e; concat.range(6,2)   = f;
    concat.range(1,0)   = 3;

    w = concat.range(31, 24);
    x = concat.range(23, 16);
    y = concat.range(15, 8);
    z = concat.range(7, 0);
}
