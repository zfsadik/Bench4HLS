void TopModule(bool x3, bool x2, bool x1, bool &f) {
    #pragma HLS INTERFACE ap_none port=x3
    #pragma HLS INTERFACE ap_none port=x2
    #pragma HLS INTERFACE ap_none port=x1
    #pragma HLS INTERFACE ap_none port=f
    #pragma HLS INTERFACE ap_ctrl_none port=return

    f = ((!x3) && x2) || (x3 && x1);
}
