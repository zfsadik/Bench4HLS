void TopModule(bool w, bool R, bool E, bool L, bool &Q) {
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=R
    #pragma HLS INTERFACE ap_none port=E
    #pragma HLS INTERFACE ap_none port=L
    #pragma HLS INTERFACE ap_none port=Q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = 0;

    // On a rising edge update the state according to the input conditions.
    if (L)
        state = R;
    else if (E)
        state = w;

    Q = state;
}
