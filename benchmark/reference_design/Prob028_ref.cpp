void TopModule(bool in1, bool in2, bool in3, bool &out) {

    #pragma HLS INTERFACE ap_none port=in1
    #pragma HLS INTERFACE ap_none port=in2
    #pragma HLS INTERFACE ap_none port=in3
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = (!(in1 ^ in2)) ^ in3;
}
