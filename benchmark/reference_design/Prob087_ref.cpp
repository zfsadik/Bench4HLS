void TopModule(bool a, bool b, bool &q) {
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return

    q = a && b;  // q is 1 only if both a and b are 1.
}
