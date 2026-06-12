#include "ap_int.h"

void TopModule(ap_uint<3> sel,
               ap_uint<4> data0,
               ap_uint<4> data1,
               ap_uint<4> data2,
               ap_uint<4> data3,
               ap_uint<4> data4,
               ap_uint<4> data5,
               ap_uint<4> &out) {
    #pragma HLS INTERFACE ap_none port=sel
    #pragma HLS INTERFACE ap_none port=data0
    #pragma HLS INTERFACE ap_none port=data1
    #pragma HLS INTERFACE ap_none port=data2
    #pragma HLS INTERFACE ap_none port=data3
    #pragma HLS INTERFACE ap_none port=data4
    #pragma HLS INTERFACE ap_none port=data5
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    switch (sel) {
        case 0:
            out = data0;
            break;
        case 1:
            out = data1;
            break;
        case 2:
            out = data2;
            break;
        case 3:
            out = data3;
            break;
        case 4:
            out = data4;
            break;
        case 5:
            out = data5;
            break;
        default:
            out = 0;
            break;
    }
}
