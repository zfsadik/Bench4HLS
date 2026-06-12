#include "ap_int.h"

void TopModule(ap_uint<1> in, ap_uint<1> &q) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<1> state = 0;

    state = in ^ state;

    q = state;
}
