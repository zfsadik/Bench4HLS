void TopModule(bool state, bool in, bool areset, bool &next_state, bool &out) {
#pragma HLS INTERFACE ap_none port=state
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=areset
#pragma HLS INTERFACE ap_none port=next_state
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

    if (areset) {
        next_state = true;
        out = true;
    } else {
        if (state == true) {         // Currently in state B.
            // If in is 0, next state becomes A; if in is 1, remain in B.
            next_state = (in ? true : false);
        } else {                     // Currently in state A.
            // If in is 0, next state becomes B; if in is 1, remain in A.
            next_state = (in ? false : true);
        }
        // Output is high if current state is B.
        out = state;
    }
}
