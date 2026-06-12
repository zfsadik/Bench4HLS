#include "ap_int.h"

void TopModule(bool reset, bool w, bool &z) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State encoding: A=0,B=1,C=2,D=3,E=4,F=5
    static ap_uint<3> state = 0;
    #pragma HLS RESET variable=state
    ap_uint<3> next_state;

    switch (state) {
        case 0: next_state = w ? 0 : 1; break; // A
        case 1: next_state = w ? 3 : 2; break; // B
        case 2: next_state = w ? 3 : 4; break; // C
        case 3: next_state = w ? 0 : 5; break; // D
        case 4: next_state = w ? 3 : 4; break; // E
        case 5: next_state = w ? 3 : 2; break; // F
        default: next_state = 0;    break;
    }

    if (reset) state = 0;
    else       state = next_state;

    z = (state == 4) || (state == 5);
}
