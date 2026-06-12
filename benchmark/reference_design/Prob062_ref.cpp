void TopModule(bool p1a, bool p1b, bool p1c, bool p1d, bool &p1y,
               bool p2a, bool p2b, bool p2c, bool p2d, bool &p2y) {
    #pragma HLS INTERFACE ap_none port=p1a
    #pragma HLS INTERFACE ap_none port=p1b
    #pragma HLS INTERFACE ap_none port=p1c
    #pragma HLS INTERFACE ap_none port=p1d
    #pragma HLS INTERFACE ap_none port=p1y

    #pragma HLS INTERFACE ap_none port=p2a
    #pragma HLS INTERFACE ap_none port=p2b
    #pragma HLS INTERFACE ap_none port=p2c
    #pragma HLS INTERFACE ap_none port=p2d
    #pragma HLS INTERFACE ap_none port=p2y

    #pragma HLS INTERFACE ap_ctrl_none port=return

    p1y = !(p1a && p1b && p1c && p1d);
    p2y = !(p2a && p2b && p2c && p2d);
}
