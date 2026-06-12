#include "ap_int.h"

void TopModule(ap_uint<6> y, ap_uint<1> w, ap_uint<1> &Y1, ap_uint<1> &Y3) {
#pragma HLS INTERFACE ap_none port=y
#pragma HLS INTERFACE ap_none port=w
#pragma HLS INTERFACE ap_none port=Y1
#pragma HLS INTERFACE ap_none port=Y3
#pragma HLS INTERFACE ap_ctrl_none port=return

    Y1 = (y[0] == 1) && w;

    Y3 = ((y[1] == 1) || (y[2] == 1) || (y[4] == 1) || (y[5] == 1)) && (!w);
}
