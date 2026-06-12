#include "ap_int.h"

void TopModule(bool slowena, bool reset, ap_uint<4> &q) {
    #pragma HLS INTERFACE ap_none port=slowena
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<4> state = 0;

    if (reset) {
        state = 0;
    } else if (slowena) {
        if (state >= 9)
            state = 0;
        else
            state = state + 1;
    }
    q = state;
}
