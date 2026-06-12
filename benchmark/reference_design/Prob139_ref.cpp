void TopModule(bool a, bool &p, bool &q) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=p
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool p_reg = false;
    static bool q_reg = false;
    #pragma HLS RESET variable=p_reg
    #pragma HLS RESET variable=q_reg

    //Falling‑edge
    q_reg = p_reg;
    p_reg = a;

    p = p_reg;
    q = q_reg;
}
