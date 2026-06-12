#include "ap_int.h"

void TopModule(ap_int<8> in, ap_int<32> &out) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = (ap_int<32>) in;
}
