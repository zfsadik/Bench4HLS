#include "ap_int.h"

void TopModule(bool reset, bool s, bool w, bool &z) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=s
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State encoding (3 bits):
    // A   = 0,
    // B   = 1,
    // S10 = 2, S11 = 3,
    // S20 = 4, S21 = 5, S22 = 6,
    // C   = 7  (z==1 state)
    static ap_uint<3> state = 0;
    ap_uint<3> next_state;

    // Combinational next-state logic
    switch (state) {
        case 0: // A
            next_state = s ? 1 : 0;
            break;
        case 1: // B, first w
            next_state = w ? 3 : 2; // S11 if w, else S10
            break;
        case 2: // S10, second w came 0
            next_state = w ? 5 : 4; // S21 if w, else S20
            break;
        case 3: // S11, second w came 1
            next_state = w ? 6 : 5; // S22 if w, else S21
            break;
        case 4: // S20, counted exactly 0 of first two -> third doesn't matter, go to B
            next_state = 1;
            break;
        case 5: // S21, counted exactly 1 of first two -> need w==1 for C else B
            next_state = w ? 7 : 1;
            break;
        case 6: // S22, counted exactly 2 of first two -> need w==0 for C else B
            next_state = w ? 1 : 7;
            break;
        case 7: // C (z==1). Next window always starts at B (re‑use s? but s only in A)
            next_state = 1;
            break;
        default:
            next_state = 0;
    }

    z = (state == 7);

    if (reset)
        state = 0;  // back to A
    else
        state = next_state;
}
