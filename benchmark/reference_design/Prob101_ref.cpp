#include "ap_int.h"

void TopModule(bool load, ap_uint<2> ena, ap_uint<100> data, ap_uint<100> &q) {
#pragma HLS INTERFACE ap_none port=load
#pragma HLS INTERFACE ap_none port=ena
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<100> reg_q = 0;
#pragma HLS RESET variable=reg_q

    // On the rising edge, update reg_q.
    if (load) {
        reg_q = data;  // Synchronous load.
    } else if (ena == 1) {
        reg_q = (reg_q[0], reg_q.range(99,1));
    } else if (ena == 2) {
        reg_q = (reg_q.range(98,0), reg_q[99]);
    }

    q = reg_q;
}
