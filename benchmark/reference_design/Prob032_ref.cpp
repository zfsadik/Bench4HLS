#include "ap_int.h"

void TopModule(ap_int<8> a, ap_int<8> b, ap_int<8> &s, bool &overflow) {
	
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=s
    #pragma HLS INTERFACE ap_none port=overflow
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_int<9> sum = a + b;

    s = sum.range(7, 0);

    overflow = (bool)(sum[8] ^ sum[7]);
}
