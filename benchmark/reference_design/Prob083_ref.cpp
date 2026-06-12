#include "ap_int.h"

void TopModule(bool reset, ap_uint<5>& q) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<5> state = 1;  // initial state (matches reset behavior)

    if (reset) {
        state = 1;
    } else {
        ap_uint<5> temp;

        temp[3] = state[4];   // new bit3 = old bit4
        temp[2] = state[3];   // new bit2 will be updated below
        temp[1] = state[2];   // new bit1 = old bit2
        temp[0] = state[1];   // new bit0 = old bit1

        temp[4] = state[0];   // new bit4 = old bit0
        temp[2] = temp[2] ^ state[0]; // new bit2 = (old bit3) XOR (old bit0)
        state = temp;
    }
    q = state;
}
