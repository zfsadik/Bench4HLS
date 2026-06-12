#include "ap_int.h"

void TopModule(ap_uint<1> x, ap_uint<3> y, ap_uint<1> &Y0, ap_uint<1> &z) {
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=Y0
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<4> idx = ((ap_uint<4>)y << 1) | (ap_uint<1>)x;

    switch (idx) {
        case 1:
        case 2:
        case 5:
        case 6:
        case 8:
            Y0 = 1;
            break;
        default:
            Y0 = 0;
    }

    switch (y) {
        case 3:
        case 4:
            z = 1;
            break;
        default:
            z = 0;
    }
}
