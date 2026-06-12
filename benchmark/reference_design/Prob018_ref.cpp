#include "ap_int.h"

void TopModule(ap_uint<256> in, ap_uint<8> sel, bool &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=sel
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = in[sel];
}
