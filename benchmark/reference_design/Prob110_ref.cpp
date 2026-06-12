#include "ap_int.h"

void TopModule(ap_uint<8> code, ap_uint<4> &out, ap_uint<1> &valid) {
#pragma HLS INTERFACE ap_none port=code
#pragma HLS INTERFACE ap_none port=out
#pragma HLS INTERFACE ap_none port=valid
#pragma HLS INTERFACE ap_ctrl_none port=return

    out = 0;
    valid = 0;

    switch (code) {
        case 0x45: out = 0; valid = 1; break;
        case 0x16: out = 1; valid = 1; break;
        case 0x1e: out = 2; valid = 1; break;
        case 0x26: out = 3; valid = 1; break;
        case 0x25: out = 4; valid = 1; break;
        case 0x2e: out = 5; valid = 1; break;
        case 0x36: out = 6; valid = 1; break;
        case 0x3d: out = 7; valid = 1; break;
        case 0x3e: out = 8; valid = 1; break;
        case 0x46: out = 9; valid = 1; break;
    }
}
