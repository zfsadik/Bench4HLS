#include "ap_int.h"

void TopModule(bool reset, ap_uint<3> &ena, ap_uint<16> &q) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=ena
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<16> state = 0;

    bool en1 = (state.range(3,0) == 9);             // Tens enabled when ones digit equals 9.
    bool en2 = (state.range(7,0) == 0x99);            // Hundreds enabled when tens and ones equal 99.
    bool en3 = (state.range(11,0) == 0x999);          // Thousands enabled when hundreds,tens,ones equal 999.

    if (reset) {
        state = 0;
    } else {
        // Update each digit of the BCD counter.
        ap_uint<4> d0 = state.range(3,0);
        d0 = (d0 == 9) ? ap_uint<4>(0) : ap_uint<4>(d0 + 1);
        state.range(3,0) = d0;

        ap_uint<4> d1 = state.range(7,4);
        if (en1) { d1 = (d1 == 9) ? ap_uint<4>(0) : ap_uint<4>(d1 + 1); }
        state.range(7,4) = d1;

        ap_uint<4> d2 = state.range(11,8);
        if (en2) { d2 = (d2 == 9) ? ap_uint<4>(0) : ap_uint<4>(d2 + 1); }
        state.range(11,8) = d2;

        ap_uint<4> d3 = state.range(15,12);
        if (en3) { d3 = (d3 == 9) ? ap_uint<4>(0) : ap_uint<4>(d3 + 1); }
        state.range(15,12) = d3;
    }

    // For convenience, pack enable signals for digits 1 to 3.
    bool re1 = (state.range(3,0) == 9);
    bool re2 = (state.range(7,0) == 0x99);
    bool re3 = (state.range(11,0) == 0x999);
    ena = (re3 << 2) | (re2 << 1) | re1;
    q = state;
}
