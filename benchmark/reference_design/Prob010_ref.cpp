void TopModule(bool x, bool y, bool &z) {

    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return

    z = (x ^ y) & x;
}
