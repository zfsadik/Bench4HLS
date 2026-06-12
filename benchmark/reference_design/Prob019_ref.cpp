#include "ap_int.h"

void TopModule(ap_uint<1> in1, ap_uint<1> in2, ap_uint<1> &out) {

    #pragma HLS INTERFACE ap_none port=in1
    #pragma HLS INTERFACE ap_none port=in2
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = in1 & (~in2);
}
