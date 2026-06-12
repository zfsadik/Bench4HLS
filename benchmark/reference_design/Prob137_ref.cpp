#include "ap_int.h"

void TopModule(
    bool        areset,
    bool        bump_left,
    bool        bump_right,
    bool        ground,
    bool       &walk_left,
    bool       &walk_right,
    bool       &aaah
) {
    #pragma HLS INTERFACE ap_none port=areset
    #pragma HLS INTERFACE ap_none port=bump_left
    #pragma HLS INTERFACE ap_none port=bump_right
    #pragma HLS INTERFACE ap_none port=ground
    #pragma HLS INTERFACE ap_none port=walk_left
    #pragma HLS INTERFACE ap_none port=walk_right
    #pragma HLS INTERFACE ap_none port=aaah
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    const ap_uint<2> WL    = 0;
    const ap_uint<2> WR    = 1;
    const ap_uint<2> FALLL = 2;
    const ap_uint<2> FALLR = 3;

    static ap_uint<2> state = WL;
    #pragma HLS RESET variable=state
    ap_uint<2>        next_state = WL;

    if (state == WL) {
        if (!ground)          next_state = FALLL;
        else if (bump_left)   next_state = WR;
        else                  next_state = WL;
    }
    else if (state == WR) {
        if (!ground)           next_state = FALLR;
        else if (bump_right)   next_state = WL;
        else                   next_state = WR;
    }
    else if (state == FALLL) {
        next_state = ground ? WL : FALLL;
    }
    else { // FALLR
        next_state = ground ? WR : FALLR;
    }

    // Moore outputs
    walk_left  = (state == WL);
    walk_right = (state == WR);
    aaah       = (state == FALLL) || (state == FALLR);

    if (areset) {
        state = WL;
    } else {
        state = next_state;
    }
}
