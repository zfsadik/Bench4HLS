void TopModule(bool a, bool b, bool c, bool &w, bool &x, bool &y, bool &z) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return

    w = a;
    x = b;
    y = b;
    z = c;
}
