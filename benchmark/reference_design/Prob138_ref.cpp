#include "ap_int.h"

void TopModule(
    bool        in,
    ap_uint<10> state,
    ap_uint<10> &next_state,
    bool       &out1,
    bool       &out2
) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_none port=next_state
    #pragma HLS INTERFACE ap_none port=out1
    #pragma HLS INTERFACE ap_none port=out2
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out1 = state[8] || state[9];
    out2 = state[7] || state[9];

    next_state[0] = (!in) && (state[0] || state[1] || state[2]|| state[3] || state[4] || state[7] || state[8] || state[9]);

    next_state[1] = in && (state[0] || state[8] || state[9]);
    next_state[2] = in && state[1];
    next_state[3] = in && state[2];
    next_state[4] = in && state[3];
    next_state[5] = in && state[4];
    next_state[6] = in && state[5];
    next_state[7] = in && (state[6] || state[7]);
    next_state[8] = (!in) && state[5];
    next_state[9] = (!in) && state[6];
}
