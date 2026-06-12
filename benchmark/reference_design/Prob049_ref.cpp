#include "ap_int.h"

void TopModule(ap_uint<4> in, ap_uint<1> &out_and, ap_uint<1> &out_or, ap_uint<1> &out_xor) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out_and
    #pragma HLS INTERFACE ap_none port=out_or
    #pragma HLS INTERFACE ap_none port=out_xor
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out_and = in.and_reduce();

    out_or = in.or_reduce();

    out_xor = in.xor_reduce();
}
