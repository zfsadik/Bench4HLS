#include "ap_int.h"

void TopModule(bool sresetn, bool x, bool & z) {
    #pragma HLS INTERFACE ap_none port=sresetn
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State encoding: S=0, S1=1, S10=2.
    static ap_uint<2> state = 0;  // initial state: S
    #pragma HLS RESET variable=state
    ap_uint<2> next_state;

    z = (state == 2) ? x : 0;

    switch (state) {
        case 0: // S: if x==1 then transition to S1; else remain in S.
            next_state = x ? 1 : 0;
            break;
        case 1: // S1: if x==1, remain in S1; if x==0, transition to S10.
            next_state = x ? 1 : 2;
            break;
        case 2: // S10: in this state, output equals x; if x==1, transition to S1; if x==0, return to S.
            next_state = x ? 1 : 0;
            break;
        default:
            next_state = 0;
            break;
    }

    if (!sresetn) {
        state = 0;
    }
    else {
        state = next_state;
    }
}
