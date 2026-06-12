#include <ap_int.h>

void TopModule(ap_uint<1> in, ap_uint<1> &out) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out = in;
}
