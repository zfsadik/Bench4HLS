#include "ap_int.h"

void TopModule(ap_uint<100> in, ap_uint<100> &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL  // Fully unroll for combinational implementation.
        out[i] = in[100 - i - 1];
    }
}
