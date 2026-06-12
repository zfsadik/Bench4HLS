#include "ap_int.h"

void TopModule(bool load, bool ena, ap_uint<2> amount, ap_uint<64> data, ap_uint<64> &q) {
#pragma HLS INTERFACE ap_none port=load
#pragma HLS INTERFACE ap_none port=ena
#pragma HLS INTERFACE ap_none port=amount
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<64> q_reg = 0;
#pragma HLS RESET variable=q_reg

    if (load) {
        q_reg = data;
    } else if (ena) {
        if (amount == 0x0) {
            // Shift left by 1: discard MSB, insert 0 at LSB.
            q_reg = (q_reg << 1);
        } else if (amount == 0x1) {
            // Shift left by 8 bits.
            q_reg = (q_reg << 8);
        } else if (amount == 0x2) {
            // Arithmetic shift right by 1:
            q_reg = (ap_uint<64>)((ap_int<64>)q_reg >> 1);
        } else if (amount == 0x3) {
            // Arithmetic shift right by 8 bits:
            q_reg = (ap_uint<64>)((ap_int<64>)q_reg >> 8);
        }
        // If ena is high but amount does not match any case, q_reg remains unchanged.
    }
    q = q_reg;
}
