#include "ap_int.h"

void TopModule(bool a, ap_uint<3> &q) {
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<3> q_reg = 0;
#pragma HLS RESET variable=q_reg

    // On a rising edge of the clock, update q_reg according to the specified behavior.
    if (a) {
        q_reg = 4;        // If a is 1, load the value 4.
    } else if (q_reg == 6) {
        q_reg = 0;        // If q equals 6, wrap around to 0.
    } else {
        q_reg = q_reg + 1; // Otherwise, increment q_reg by 1.
    }
    q = q_reg;
}
