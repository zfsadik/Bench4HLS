#include "ap_int.h"

void TopModule(bool in, ap_uint<2> state, ap_uint<2> &next_state, bool &out) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=state
#pragma HLS INTERFACE ap_none port=next_state
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

    if (state == 0) {
        // State A
        next_state = (in ? 1 : 0);
    } else if (state == 1) {
        // State B
        next_state = (in ? 1 : 2);
    } else if (state == 2) {
        // State C
        next_state = (in ? 3 : 0);
    } else { // state == 3, State D
        next_state = (in ? 1 : 2);
    }

    out = (state == 3);
}
