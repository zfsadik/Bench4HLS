#include <ap_int.h>

void TopModule(
    bool a,
    bool b,
    bool &q,
    bool &state
) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool c = 0;

    bool c_next = (a && b) || (a && c) || (b && c);
    q     = a ^ b ^ c;
    state = c;
    c     = c_next;
}
