#include "ap_int.h"

void TopModule(ap_uint<1024> in, ap_uint<8> sel, ap_uint<4> &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=sel
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<10> base = sel * 4;
    out = in.range(base + 3, base);
}
