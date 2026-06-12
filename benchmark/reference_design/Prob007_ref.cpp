void TopModule(bool in, bool &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = in;
}
