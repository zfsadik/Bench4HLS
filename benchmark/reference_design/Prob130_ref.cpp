#include <ap_int.h>

void TopModule(ap_uint<3> y, ap_uint<1> w, ap_uint<1> &Y2) {
    #pragma HLS INTERFACE ap_none port=y
    #pragma HLS INTERFACE ap_none port=w
    #pragma HLS INTERFACE ap_none port=Y2
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<4> idx = (ap_uint<4>(y) << 1) | (ap_uint<1>)w;

    switch (idx) {
        case 0x0: Y2 = 0; break;
        case 0x1: Y2 = 0; break;
        case 0x2: Y2 = 0; break;
        case 0x3: Y2 = 0; break;
        case 0x4: Y2 = 1; break;
        case 0x5: Y2 = 0; break;
        case 0x6: Y2 = 1; break;
        case 0x7: Y2 = 0; break;
        case 0x8: Y2 = 1; break;
        case 0x9: Y2 = 0; break;
        case 0xA: Y2 = 0; break;
        case 0xB: Y2 = 0; break;
        default: Y2 = 0;
    }
}
