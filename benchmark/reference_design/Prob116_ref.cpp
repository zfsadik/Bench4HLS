#include "ap_int.h"

void TopModule(bool in, bool reset, bool &out) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<2> state = 0;  // Initial state; reset will force state = A (0).
#pragma HLS RESET variable=state

    out = (state == 3);

    if (reset) {
        state = 0;  // Reset to A.
    } else {
        ap_uint<2> next;
        if (state == 0) {  // State A
            if (in == true) {
                next = 1;  // A --(in=1)--> B
            } else {
                next = 0;  // A --(in=0)--> A
            }
        } else if (state == 1) {  // State B
            if (in == true) {
                next = 1;  // B --(in=1)--> B
            } else {
                next = 2;  // B --(in=0)--> C
            }
        } else if (state == 2) {  // State C
            if (in == true) {
                next = 3;  // C --(in=1)--> D
            } else {
                next = 0;  // C --(in=0)--> A
            }
        } else {  // state == 3, State D
            if (in == true) {
                next = 1;  // D --(in=1)--> B
            } else {
                next = 2;  // D --(in=0)--> C
            }
        }
        state = next;
    }
}
