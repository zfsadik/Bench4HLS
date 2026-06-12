#include "ap_int.h"

void TopModule(ap_uint<2> A, ap_uint<2> B, bool &z) {

    #pragma HLS INTERFACE ap_none port=A
    #pragma HLS INTERFACE ap_none port=B
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS LATENCY min=0 max=0

    z = (A == B);
}
