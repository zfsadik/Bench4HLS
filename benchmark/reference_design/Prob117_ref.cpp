#include "ap_int.h"

void TopModule(bool reset, bool x, bool &z) {
  #pragma HLS INTERFACE ap_none port=reset
  #pragma HLS INTERFACE ap_none port=x
  #pragma HLS INTERFACE ap_none port=z
  #pragma HLS INTERFACE ap_ctrl_none port=return
  #pragma HLS PIPELINE II=1

    static ap_uint<3> state = 0; // Initially state A.
    #pragma HLS RESET variable=state

    z = ((state == 3) || (state == 4));

    if (reset) {
        state = 0;  // Reset to state A.
    } else {
        ap_uint<3> next;
        if (state == 0) {  // State A (000)
            if (x == true)
                next = 1;  // A: if x==1 then transition to B (001)
            else
                next = 0;  // A: if x==0 then remain in A.
        } else if (state == 1) { // State B (001)
            if (x == true)
                next = 4;  // B: if x==1 then transition to E (100)
            else
                next = 1;  // B: if x==0 then remain in B.
        } else if (state == 2) { // State C (010)
            if (x == true)
                next = 1;  // C: if x==1 then transition to B (001)
            else
                next = 2;  // C: if x==0 then remain in C.
        } else if (state == 3) { // State D (011)
            if (x == true)
                next = 2;  // D: if x==1 then transition to C (010)
            else
                next = 1;  // D: if x==0 then transition to B (001)
        } else {               // State E (100)
            if (x == true)
                next = 4;  // E: if x==1 then remain in E.
            else
                next = 3;  // E: if x==0 then transition to D (011)
        }
        state = next;
    }
}
