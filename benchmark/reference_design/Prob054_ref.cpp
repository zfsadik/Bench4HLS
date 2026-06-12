void TopModule(bool j, bool k, bool &Q) {

    #pragma HLS INTERFACE ap_none port=j
    #pragma HLS INTERFACE ap_none port=k
    #pragma HLS INTERFACE ap_none port=Q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = 0;

    state = (j && (!state)) || ((!k) && state);
    Q = state;
}
