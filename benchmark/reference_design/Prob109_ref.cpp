#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<1> &f) {
#pragma HLS INTERFACE ap_none port=x
#pragma HLS INTERFACE ap_none port=f
#pragma HLS INTERFACE ap_ctrl_none port=return

    f = (!x[2] && !x[0]) || (x[3] && !x[1]) || (x[3] && x[2] && x[0]);
}
