#include "ap_int.h"

void TopModule(bool reset, ap_uint<32> &q) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<32> q_reg = 1;

    if (reset) {
        q_reg = 1;
    } else {
        ap_uint<32> q_next;

        q_next = q_reg >> 1;

        q_next[31] = q_reg[0];

        q_next[21] = q_next[21] ^ q_reg[0];
        q_next[1]  = q_next[1]  ^ q_reg[0];
        q_next[0]  = q_next[0]  ^ q_reg[0];
        q_reg = q_next;
    }
    q = q_reg;
}
