#include "ap_int.h"

void TopModule(ap_uint<3> a, ap_uint<3> b, 
               ap_uint<3> &out_or_bitwise, bool &out_or_logical, 
               ap_uint<6> &out_not) {

    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=out_or_bitwise
    #pragma HLS INTERFACE ap_none port=out_or_logical
    #pragma HLS INTERFACE ap_none port=out_not
    #pragma HLS INTERFACE ap_ctrl_none port=return

    out_or_bitwise = a | b;
    out_or_logical = (a | b).or_reduce();

    ap_uint<3> not_a = ~a;
    ap_uint<3> not_b = ~b;

    out_not = (not_b, not_a);
}
