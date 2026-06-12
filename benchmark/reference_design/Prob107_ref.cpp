#include "ap_int.h"

void TopModule(bool j, bool k, bool reset, bool &out) {
#pragma HLS INTERFACE ap_none port=j
#pragma HLS INTERFACE ap_none port=k
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static bool state = false;     // initial state (will be overridden by reset)
#pragma HLS RESET variable=state

    out = state;

    // On rising edge of clk, update state.
    if (reset) {
        state = false;  // Synchronous reset: force state to OFF.
    } else {
        if (state == false) {  // Currently OFF
            if (j == true) {
                state = true;  // Transition to ON when j is 1.
            }
            else {
                state = false; // Remain OFF when j is 0.
            }
        }
        else {  // Currently ON
            if (k == true) {
                state = false; // Transition to OFF when k is 1.
            }
            else {
                state = true;  // Remain ON when k is 0.
            }
        }
    }
}
