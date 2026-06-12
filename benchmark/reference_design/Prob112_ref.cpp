#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<1> &f) {
#pragma HLS INTERFACE ap_none port=x
#pragma HLS INTERFACE ap_none port=f
#pragma HLS INTERFACE ap_ctrl_none port=return

    bool b1 = x[3], b2 = x[2], b3 = x[1];
    f = (!b1 & b3) | (b1 & b2 & !b3);
}
