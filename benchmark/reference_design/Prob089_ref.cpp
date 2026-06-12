#include "ap_int.h"

void TopModule(ap_uint<100> in, 
               ap_uint<99> &out_both, 
               ap_uint<99> &out_any, 
               ap_uint<100> &out_different) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=out_both
#pragma HLS INTERFACE ap_none port=out_any
#pragma HLS INTERFACE ap_none port=out_different
#pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<99> in_right = in.range(99, 1);
    ap_uint<99> in_left = in.range(98, 0);
    
    out_both = in_left & in_right;
    
    out_any = in_left | in_right;
    
    ap_uint<100> rotated = ((ap_uint<100>)(in[0]) << 99) | in.range(99, 1);
    out_different = in ^ rotated;
}
