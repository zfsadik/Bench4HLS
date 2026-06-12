#include "ap_int.h"

void TopModule(bool enable, bool S, bool A, bool B, bool C, bool &Z) {
    #pragma HLS INTERFACE ap_none port=enable
    #pragma HLS INTERFACE ap_none port=S
    #pragma HLS INTERFACE ap_none port=A
    #pragma HLS INTERFACE ap_none port=B
    #pragma HLS INTERFACE ap_none port=C
    #pragma HLS INTERFACE ap_none port=Z
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<8> q_reg = 0; // Initialize to 0

    if (enable) {
        q_reg = (q_reg << 1) | S;
    }

    ap_uint<3> addr;
    addr[2] = A;  // MSB of address
    addr[1] = B;
    addr[0] = C;  // LSB of address

    Z = (q_reg >> addr) & 1;
}
