#include "ap_int.h"

void TopModule(ap_uint<1> in, ap_uint<1> reset, ap_uint<1> &out) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<2> state = 0; // Initial value (A) – reset forces state to A.
    ap_uint<2> next;

    if (state == 0) {          // State A
        if (in) {
            next = 1;        // Transition to B when in==1.
        } else {
            next = 0;        // Remain in A when in==0.
        }
    }
    else if (state == 1) {     // State B
        if (in) {
            next = 1;        // Remain in B when in==1.
        } else {
            next = 2;        // Transition to C when in==0.
        }
    }
    else if (state == 2) {     // State C
        if (in) {
            next = 3;        // Transition to D when in==1.
        } else {
            next = 0;        // Transition to A when in==0.
        }
    }
    else {                     // state == 3, State D
        if (in) {
            next = 1;        // Transition to B when in==1.
        } else {
            next = 2;        // Transition to C when in==0.
        }
    }

    if (reset) {
        state = 0;
    }
    else {
        state = next;
    }

    out = (state == 3);
}
