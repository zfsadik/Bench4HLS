#include "ap_int.h"

void TopModule(bool d, bool &q) {

    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = 0;      // Initial state (can be set to 0)

    q = state;

    state = d;
}
