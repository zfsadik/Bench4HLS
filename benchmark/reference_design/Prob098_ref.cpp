void TopModule(bool a, bool b, bool c, bool d, bool &q) {
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_none port=c
#pragma HLS INTERFACE ap_none port=d
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return

    // The circuit is determined from the waveforms to output q = c OR b.
    (void)a;
    (void)d;
    q = c || b;
}
