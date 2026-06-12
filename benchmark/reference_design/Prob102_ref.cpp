#include "ap_int.h"

void TopModule(ap_uint<16> scancode, bool &left, bool &down, bool &right, bool &up) {
#pragma HLS INTERFACE ap_none port=scancode
#pragma HLS INTERFACE ap_none port=left
#pragma HLS INTERFACE ap_none port=down
#pragma HLS INTERFACE ap_none port=right
#pragma HLS INTERFACE ap_none port=up
#pragma HLS INTERFACE ap_ctrl_none port=return

    left  = (scancode == 0xE06B);
    down  = (scancode == 0xE072);
    right = (scancode == 0xE074);
    up    = (scancode == 0xE075);
}
