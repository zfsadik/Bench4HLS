#include "ap_int.h"

void TopModule(bool shift_ena, bool count_ena, bool data, ap_uint<4> &q) {
    #pragma HLS INTERFACE ap_none port=shift_ena
    #pragma HLS INTERFACE ap_none port=count_ena
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<4> state = 0;

    if (shift_ena) {
        state = (state << 1) | data;
    }
    else if (count_ena) {
        state = state - 1;
    }

    q = state;
}
