void TopModule(bool in, bool state, bool reset, bool &next_state, bool &out) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=state
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=next_state
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

    out = state;

    if (reset) {
        next_state = true;  // Synchronous reset: state = B (output 1)
    } else {
        if (!state) {  // Currently state A
            next_state = (in ? false : true);
        } else {       // Currently state B
            next_state = (in ? true : false);
        }
    }
}
