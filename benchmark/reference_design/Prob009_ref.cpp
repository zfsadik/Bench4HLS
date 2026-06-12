#include "ap_int.h"

void TopModule(ap_uint<3> in, ap_uint<2>& out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS LATENCY min=0 max=0

    out = in[0] + in[1] + in[2];
}
