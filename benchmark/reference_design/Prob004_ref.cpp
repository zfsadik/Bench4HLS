#include "ap_int.h"

void TopModule(ap_uint<32> in, ap_uint<32>& out) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = (in.range(7, 0), in.range(15, 8), in.range(23, 16), in.range(31, 24));
}
