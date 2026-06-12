#include "ap_int.h"

void TopModule(ap_uint<1>& one) {
    #pragma HLS INTERFACE ap_none port=one
    #pragma HLS INTERFACE ap_ctrl_none port=return

    one = 1;
}
