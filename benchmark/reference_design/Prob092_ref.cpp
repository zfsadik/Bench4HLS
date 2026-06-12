#include "ap_int.h"

#define B0 0
#define B1 1
#define B2 2
#define B3 3
#define DONE 4

void TopModule(bool reset, bool &shift_ena) {
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=shift_ena
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<3> state = DONE;

    if (reset) {
        state = B0;  // Synchronous reset: force state to B0.
    } else {
        // Update state based on current state.
        switch(state) {
            case B0: state = B1; break;
            case B1: state = B2; break;
            case B2: state = B3; break;
            case B3: state = DONE; break;
            case DONE: state = DONE; break;
            default: state = DONE; break;
        }
    }

    // shift_ena is true if state is one of B0, B1, B2, or B3.
    shift_ena = (state < DONE);
}
