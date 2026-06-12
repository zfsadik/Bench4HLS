#include <ap_int.h>

void TopModule(
    bool          resetn,
    ap_uint<2>    byteena,
    ap_uint<16>   d,
    ap_uint<16> & q
) {
#pragma HLS INTERFACE ap_none port=resetn
#pragma HLS INTERFACE ap_none port=byteena
#pragma HLS INTERFACE ap_none port=d
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<16> q_reg = 0;

    if (!resetn) {
        q_reg = 0;
    } else {
        if (byteena[0]) q_reg.range(7,0)   = d.range(7,0);
        if (byteena[1]) q_reg.range(15,8)  = d.range(15,8);
    }

    q = q_reg;
}
