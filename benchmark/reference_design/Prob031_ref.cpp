#include "ap_int.h"

void TopModule(ap_uint<3> vec, ap_uint<3> &outv, bool &o2, bool &o1, bool &o0) {

    #pragma HLS INTERFACE ap_none port=vec
    #pragma HLS INTERFACE ap_none port=outv
    #pragma HLS INTERFACE ap_none port=o2
    #pragma HLS INTERFACE ap_none port=o1
    #pragma HLS INTERFACE ap_none port=o0
    #pragma HLS INTERFACE ap_ctrl_none port=return

    outv = vec;

    o2 = vec[2];
    o1 = vec[1];
    o0 = vec[0];
}
