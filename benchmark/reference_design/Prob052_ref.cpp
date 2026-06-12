#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<8> &pedge) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=pedge
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<8> d_last = 0;

    pedge = in & ~d_last;
    d_last = in;
}
