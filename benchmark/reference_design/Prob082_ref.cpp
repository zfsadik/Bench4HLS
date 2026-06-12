#include "ap_int.h"

void TopModule(bool reset, bool load, bool ena, ap_uint<4> data, ap_uint<4> &q) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=load
    #pragma HLS INTERFACE ap_none port=ena
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<4> q_reg = 0;

    if (reset) {
        q_reg = 0;
    } else if (load) {
        q_reg = data;
    } else if (ena) {
        q_reg = q_reg >> 1;
    }
    q = q_reg;
}
