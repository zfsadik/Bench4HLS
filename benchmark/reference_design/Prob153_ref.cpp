#include "ap_fixed.h"

void TopModule(
    ap_fixed<8,1>       x,
    const ap_fixed<8,1> h[4],
    ap_fixed<28,7>     &y,
    bool               &valid
) {
#pragma HLS INTERFACE ap_none port=x
#pragma HLS INTERFACE ap_none port=h
#pragma HLS INTERFACE ap_none port=y
#pragma HLS INTERFACE ap_none port=valid
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1
#pragma HLS LATENCY min=1 max=1

    // === stage-0 state ===
    static ap_fixed<8,1>   regs0[4] = {0,0,0,0};
#pragma HLS ARRAY_PARTITION variable=regs0 complete dim=1
    static bool            tog0     = false;
    static ap_fixed<19,4>  y0_int   = 0;
    static bool            f0_vld   = false;

    // === stage-1 state ===
    static ap_fixed<19,4>  regs1[4] = {0,0,0,0};
#pragma HLS ARRAY_PARTITION variable=regs1 complete dim=1
    static bool            tog1     = false;
    static ap_fixed<28,7>  y1_int   = 0;
    bool                   f1_vld   = false;

    // scheduler
    static ap_uint<2> phase_cnt = 0;
    ap_uint<2>       sel_phase;
    if (phase_cnt == 0)       sel_phase = 0;  // phase 0 → stage-0
    else if (phase_cnt == 3)  sel_phase = 1;  // phase 3 → stage-1
    else                       sel_phase = 3;  // idle

    // --- Stage-0 feed ---
    if (sel_phase == 0) {
            // shift
            regs0[3]=regs0[2]; regs0[2]=regs0[1];
            regs0[1]=regs0[0]; regs0[0]=x;
            // MAC
            ap_fixed<19,4> acc0 = 0;
        MAC0: for (int i = 0; i < 4; ++i) {
#pragma HLS UNROLL
                acc0 += regs0[i] * h[i];
            }
            // decimate-by-2
            tog0 = !tog0;
            if (tog0) {
                y0_int = acc0;
                f0_vld = true;
            }
    }

    // --- Stage-1 feed ---
    if (sel_phase == 1 && f0_vld) {
        f0_vld = false;
            // shift
            regs1[3]=regs1[2]; regs1[2]=regs1[1];
            regs1[1]=regs1[0]; regs1[0]=y0_int;
            // MAC
            ap_fixed<28,7> acc1 = 0;
        MAC1: for (int i = 0; i < 4; ++i) {
#pragma HLS UNROLL
                acc1 += regs1[i] * h[i];
            }
            // decimate-by-2
            tog1 = !tog1;
            if (tog1) {
                y1_int = acc1;
                f1_vld = true;
            }
    }

    // output
    y     = y1_int;
    valid = f1_vld;

    phase_cnt++;
}
