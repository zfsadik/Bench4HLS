void TopModule(bool d, bool &q) {
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool qp = 0;   // Updated on positive edge

    q = qp;
    qp = d;
}
