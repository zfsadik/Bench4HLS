void TopModule(bool d, bool sr, bool &q) {
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=sr
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = 0;

    // Synchronous reset: if sr is high on the rising edge, clear the state.
    if (sr) {
        state = 0;
    }
    // Otherwise, update the state with the data input.
    else {
        state = d;
    }

    q = state;
}
