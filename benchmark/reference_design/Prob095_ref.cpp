void TopModule(bool a, bool &q) {
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static bool reg_q = false;

    q = reg_q;  // Drive output.
    reg_q = !a;  // On posedge, update q with inverted value of a.
}
