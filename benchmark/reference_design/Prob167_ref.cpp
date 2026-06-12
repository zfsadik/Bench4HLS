#include "ap_int.h"

void TopModule(ap_int<32> in[8], ap_int<32> out[8], bool &valid) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_none port=valid
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS ARRAY_PARTITION variable=in complete dim=1
#pragma HLS ARRAY_PARTITION variable=out complete dim=1

    ap_int<32> a[8];
#pragma HLS ARRAY_PARTITION variable=a complete dim=1

    for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
        a[i] = in[i];
    }

    for (int i = 0; i < 7; i++) {
#pragma HLS UNROLL
        for (int j = i+1; j < 8; j++) {
#pragma HLS UNROLL
            if (a[i] > a[j]) {
                ap_int<32> tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
        out[i] = a[i];
    }

    valid = true;
}
