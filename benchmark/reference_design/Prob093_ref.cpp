#include "ap_int.h"

#define S     0
#define S1    1
#define S11   2
#define S110  3
#define DONE  4

void TopModule(bool reset, bool data, bool &start_shifting) {
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=start_shifting
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<3> state = S;
#pragma HLS RESET variable=state

    ap_uint<3> cur_state = state;

    if (reset) {
        state = S;  // Synchronous reset.
    } else {
        switch(cur_state) {
            case S:
                state = (data ? S1 : S);
                break;
            case S1:
                state = (data ? S11 : S);
                break;
            case S11:
                state = (data ? S11 : S110);
                break;
            case S110:
                state = (data ? DONE : S);
                break;
            case DONE:
                state = DONE;
                break;
            default:
                state = DONE;
                break;
        }
    }

    start_shifting = (cur_state == DONE);
}
