#include <ap_int.h>

void TopModule(ap_uint<1> x, ap_uint<1> y, ap_uint<1> &z) {
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return

    z = !(x ^ y);
}
