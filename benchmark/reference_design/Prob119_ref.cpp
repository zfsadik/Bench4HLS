#include "ap_int.h"

void TopModule(bool do_sub, ap_uint<8> a, ap_uint<8> b, ap_uint<8> &out, bool &result_is_zero) {
    #pragma HLS INTERFACE ap_none port=do_sub
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_none port=result_is_zero
    #pragma HLS INTERFACE ap_ctrl_none port=return

    if (do_sub == false) {
        out = a + b;
    } else {
        out = a - b;
    }
    result_is_zero = (out == 0);
}
