#include "ap_int.h"

void TopModule(bool reset, ap_uint<32> in, ap_uint<32> &out) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<32> d_last = 0;
    static ap_uint<32> state_out = 0;

    if (reset) {
        state_out = 0;
    } else {
        ap_uint<32> event = (~in) & d_last;
        state_out = state_out | event;
    }
    d_last = in;

    out = state_out;
}
