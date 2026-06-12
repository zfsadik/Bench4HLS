#include "ap_int.h"

void TopModule(bool j, bool k, bool reset, bool &out) {
#pragma HLS INTERFACE ap_none port=j
#pragma HLS INTERFACE ap_none port=k
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static bool state = false;
#pragma HLS RESET variable=state
    bool cur_state = state;

    if (reset) {
        state = false;
    }
    else {
        if (state == false) {  // Currently OFF.
            if (j == true) {
                state = true;  // Transition to ON if j==1.
            } else {
                state = false; // Remain OFF if j==0.
            }
        }
        else {  // Currently ON.
            if (k == true) {
                state = false; // Transition to OFF if k==1.
            } else {
                state = true;  // Remain ON if k==0.
            }
        }
    }
    out = cur_state;
}
