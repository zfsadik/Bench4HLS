#include "ap_int.h"

void TopModule(bool x, bool &z) {
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<3> s = 0;

    ap_uint<1> bit2 = s[2] ^ x;
    ap_uint<1> bit1 = ((~s[1]) & x);
    ap_uint<1> bit0 = ((~s[0]) | x);

    z = !(s[2] || s[1] || s[0]);

	// Build new state from the individual bits.
    ap_uint<3> new_s = 0;
    new_s[2] = bit2;
    new_s[1] = bit1;
    new_s[0] = bit0;
    s = new_s;
}
