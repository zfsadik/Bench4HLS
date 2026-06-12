#include "ap_int.h"

void TopModule(bool reset, bool w, bool &z) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State codes: A=0, B=1, C=2, D=3, E=4, F=5
    static ap_uint<3> state = 0;
    #pragma HLS RESET variable=state
    ap_uint<3> next;

    z = (state == 4) || (state == 5);

    if (reset) {
        state = 0;
    } else {
        switch (state) {
            case 0: next = w ? 1 : 0; break; // A
            case 1: next = w ? 2 : 3; break; // B
            case 2: next = w ? 4 : 3; break; // C
            case 3: next = w ? 5 : 0; break; // D
            case 4: next = w ? 4 : 3; break; // E
            case 5: next = w ? 2 : 3; break; // F
            default: next = 0;       break;
        }
        state = next;
    }
}
