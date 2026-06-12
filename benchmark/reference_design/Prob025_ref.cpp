#include "ap_int.h"

void TopModule(ap_uint<8> in, bool &parity) {

    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=parity
    #pragma HLS INTERFACE ap_ctrl_none port=return

    parity = (bool)(in.xor_reduce());
}
