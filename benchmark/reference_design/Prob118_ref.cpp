#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, bool &f) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=c
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=f
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<4> input_val;
    input_val[3] = a;
    input_val[2] = b;
    input_val[1] = c;
    input_val[0] = d;

    switch(input_val) {
        case 0x0: f = false; break;  // 0000 → 0
        case 0x1: f = true;  break;  // 0001 → 1
        case 0x2: f = true;  break;  // 0010 → 1
        case 0x3: f = false; break;  // 0011 → 0
        case 0x4: f = true;  break;  // 0100 → 1
        case 0x5: f = false; break;  // 0101 → 0
        case 0x6: f = false; break;  // 0110 → 0
        case 0x7: f = true;  break;  // 0111 → 1
        case 0x8: f = true;  break;  // 1000 → 1
        case 0x9: f = false; break;  // 1001 → 0
        case 0xA: f = false; break;  // 1010 → 0
        case 0xB: f = true;  break;  // 1011 → 1
        case 0xC: f = false; break;  // 1100 → 0
        case 0xD: f = true;  break;  // 1101 → 1
        case 0xE: f = true;  break;  // 1110 → 1
        case 0xF: f = false; break;  // 1111 → 0
        default:  f = false; break;
    }
}
