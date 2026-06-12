#include "ap_int.h"

void TopModule(ap_uint<1> p1a, ap_uint<1> p1b, ap_uint<1> p1c,
               ap_uint<1> p1d, ap_uint<1> p1e, ap_uint<1> p1f,
               ap_uint<1> p2a, ap_uint<1> p2b, ap_uint<1> p2c, ap_uint<1> p2d,
               ap_uint<1> &p1y, ap_uint<1> &p2y) {
    #pragma HLS INTERFACE ap_none port=p1a
    #pragma HLS INTERFACE ap_none port=p1b
    #pragma HLS INTERFACE ap_none port=p1c
    #pragma HLS INTERFACE ap_none port=p1d
    #pragma HLS INTERFACE ap_none port=p1e
    #pragma HLS INTERFACE ap_none port=p1f
    #pragma HLS INTERFACE ap_none port=p2a
    #pragma HLS INTERFACE ap_none port=p2b
    #pragma HLS INTERFACE ap_none port=p2c
    #pragma HLS INTERFACE ap_none port=p2d
    #pragma HLS INTERFACE ap_none port=p1y
    #pragma HLS INTERFACE ap_none port=p2y
    #pragma HLS INTERFACE ap_ctrl_none port=return

    bool and1 = p1a && p1b && p1c;
    bool and2 = p1d && p1e && p1f;
    p1y = and1 || and2;

    bool and3 = p2a && p2b;
    bool and4 = p2c && p2d;
    p2y = and3 || and4;
}
