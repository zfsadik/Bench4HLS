#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<3> &pos) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=pos
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    if      (in[0]) pos = 0;
    else if (in[1]) pos = 1;
    else if (in[2]) pos = 2;
    else if (in[3]) pos = 3;
    else if (in[4]) pos = 4;
    else if (in[5]) pos = 5;
    else if (in[6]) pos = 6;
    else if (in[7]) pos = 7;
    else            pos = 0;
}