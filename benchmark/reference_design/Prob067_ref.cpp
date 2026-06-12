#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool d, bool &out_sop, bool &out_pos) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_none port=c
#pragma HLS INTERFACE ap_none port=d
#pragma HLS INTERFACE ap_none port=out_sop
#pragma HLS INTERFACE ap_none port=out_pos

    out_sop = (c && d) || ((!a) && (!b) && c);
    out_pos = c && ((!b) || d) && ((!a) || b);
}
