void TopModule(bool d, bool r, bool &q) {
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=r
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = 0;

    if (r)
        state = 0;
    else
        state = d;

    q = state;
}
