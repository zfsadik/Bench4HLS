#include "ap_int.h"

void TopModule(ap_uint<255> in, ap_uint<8> &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<8> sum = 0;

    for (int i = 0; i < 255; i++) {
        #pragma HLS UNROLL
        sum += in[i];  
    }
    out = sum;
}
