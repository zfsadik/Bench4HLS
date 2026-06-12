void TopModule(bool a, bool b, bool &sum, bool &cout) {

    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=sum
    #pragma HLS INTERFACE ap_none port=cout
    #pragma HLS INTERFACE ap_ctrl_none port=return

    sum = a ^ b;
    cout = a & b;
}
