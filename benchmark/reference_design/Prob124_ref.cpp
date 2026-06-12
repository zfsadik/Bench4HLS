#include "ap_int.h"

void TopModule(ap_uint<8> in, bool reset, bool & done) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State encoding: 0: BYTE1, 1: BYTE2, 2: BYTE3, 3: DONE.
    static ap_uint<2> state = 0;
    #pragma HLS RESET variable=state
    ap_uint<2> next_state;
    bool in3 = in[3];

    switch (state) {
        case 0: // BYTE1: Look for byte with in[3]==1.
            next_state = in3 ? 1 : 0;
            break;
        case 1: // BYTE2: Received first byte; next is BYTE3.
            next_state = 2;
            break;
        case 2: // BYTE3: Received second byte; next is DONE.
            next_state = 3;
            break;
        case 3: // DONE: Assert done output. Then, if in[3]==1, assume a new message starts (BYTE2); else, remain in BYTE1.
            next_state = in3 ? 1 : 0;
            break;
        default:
            next_state = 0;
            break;
    }

    done = (state == 3);

    if (reset)
        state = 0;  // Reset to BYTE1.
    else
        state = next_state;
}
