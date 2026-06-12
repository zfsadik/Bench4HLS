#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool d, bool e, ap_uint<25> &out) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=e
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    bool in_arr[5] = { a, b, c, d, e };
    #pragma HLS ARRAY_PARTITION variable=in_arr complete

    ap_uint<25> result = 0;
    for (int row = 0; row < 5; row++) {
        #pragma HLS UNROLL
        for (int col = 0; col < 5; col++) {
            #pragma HLS UNROLL
            result[24 - (row * 5 + col)] = (bool)(in_arr[row] == in_arr[col]);
        }
    }
    out = result;
}
