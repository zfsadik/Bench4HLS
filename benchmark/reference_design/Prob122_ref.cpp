#include "ap_int.h"

void TopModule(ap_uint<3> a, ap_uint<16> & q) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return

    switch(a) {
        case 0: q = 4658;  break; // 4658 decimal = 0x1232
        case 1: q = 44768; break; // 44768 decimal = 0xAEE0
        case 2: q = 10196; break; // 10196 decimal = 0x27D4
        case 3: q = 23054; break; // 23054 decimal = 0x5A0E
        case 4: q = 8294;  break; // 8294  decimal = 0x2066
        case 5: q = 25806; break; // 25806 decimal = 0x64CE
        case 6: q = 50470; break; // 50470 decimal = 0xC526
        case 7: q = 12057; break; // 12057 decimal = 0x2F19
        default: q = 0;     break;
    }
}
