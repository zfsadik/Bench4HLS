#include "ap_int.h"

void TopModule(bool reset, bool x, bool &z) {
	#pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=reset
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS PIPELINE II=1

    // State encoding: A=0, B=1, C=2.
    static ap_uint<2> state = 0;

    // Synchronous reset (active-high).
    if (reset) {
        state = 0; // Set state to A.
    }
    else {
        switch(state) {
            case 0: // In state A.
                state = (x ? 2 : 0);
                break;
            case 1: // In state B.
                state = (x ? 1 : 2);
                break;
            case 2: // In state C.
                state = (x ? 1 : 2);
                break;
            default:
                state = 0;
                break;
        }
    }

    z = (state == 2);
}
