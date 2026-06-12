#include "ap_int.h"

void TopModule(ap_uint<8> d, ap_uint<8> &q) {

    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<8> state = 0;

    q = state;
    state = d;
}
