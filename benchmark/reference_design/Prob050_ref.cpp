#include "ap_int.h"

void TopModule(ap_uint<100> in, bool &out_and, bool &out_or, bool &out_xor) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out_and
    #pragma HLS INTERFACE ap_none port=out_or
    #pragma HLS INTERFACE ap_none port=out_xor
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out_and = in.and_reduce();
    out_or  = in.or_reduce();
    out_xor = in.xor_reduce();
}
