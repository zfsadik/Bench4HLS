#include "ap_int.h"

void TopModule(ap_uint<1>& zero) {
    #pragma HLS INTERFACE ap_none port=zero
    #pragma HLS INTERFACE ap_ctrl_none port=return

    zero = 0;
}
