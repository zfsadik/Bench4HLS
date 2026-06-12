#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8>& out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    for (int i = 0; i < 8; i++) {
        #pragma HLS UNROLL
        out[i] = in[7 - i];
    }
}
