#include "ap_int.h"

void TopModule(ap_uint<4> in, ap_uint<2> &pos) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=pos
#pragma HLS INTERFACE ap_ctrl_none port=return

    if      (in[0]) pos = 0;
    else if (in[1]) pos = 1;
    else if (in[2]) pos = 2;
    else if (in[3]) pos = 3;
    else            pos = 0;
}
