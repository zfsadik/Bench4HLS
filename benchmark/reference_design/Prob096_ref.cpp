#include "ap_int.h"

void TopModule(ap_uint<6> y, bool w, bool &Y2, bool &Y4) {
#pragma HLS INTERFACE ap_none port=y
#pragma HLS INTERFACE ap_none port=w
#pragma HLS INTERFACE ap_none port=Y2
#pragma HLS INTERFACE ap_none port=Y4
#pragma HLS INTERFACE ap_ctrl_none port=return

    Y2 = y[0] & (!w);
    Y4 = (y[1] | y[2] | y[4] | y[5]) & w;
}
