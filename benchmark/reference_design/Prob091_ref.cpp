#include "ap_int.h"

void TopModule(ap_uint<4> in, 
               ap_uint<3> &out_both, 
               ap_uint<3> &out_any, 
               ap_uint<4> &out_different) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=out_both
#pragma HLS INTERFACE ap_none port=out_any
#pragma HLS INTERFACE ap_none port=out_different
#pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<3> in_lower = in.range(2, 0);
    ap_uint<3> in_upper = in.range(3, 1);

    out_both = in_lower & in_upper;

    out_any = in_lower | in_upper;

    ap_uint<4> rotated;
    rotated[3] = in[0];
    rotated.range(2, 0) = in.range(3, 1);
    out_different = in ^ rotated;
}
