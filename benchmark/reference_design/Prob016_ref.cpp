#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<4> y, ap_uint<5> &sum) {

    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=sum
    #pragma HLS INTERFACE ap_ctrl_none port=return

    sum = x + y;
}
