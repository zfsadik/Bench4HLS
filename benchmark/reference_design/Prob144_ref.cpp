#include <ap_int.h>

void TopModule(
    bool        d,
    bool        done_counting,
    bool        ack,
    ap_uint<10> state,
    bool       &B3_next,
    bool       &S_next,
    bool       &S1_next,
    bool       &Count_next,
    bool       &Wait_next,
    bool       &done,
    bool       &counting,
    bool       &shift_ena
) {
    #pragma HLS INTERFACE ap_none port=d
    #pragma HLS INTERFACE ap_none port=done_counting
    #pragma HLS INTERFACE ap_none port=ack
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_none port=B3_next
    #pragma HLS INTERFACE ap_none port=S_next
    #pragma HLS INTERFACE ap_none port=S1_next
    #pragma HLS INTERFACE ap_none port=Count_next
    #pragma HLS INTERFACE ap_none port=Wait_next
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_none port=counting
    #pragma HLS INTERFACE ap_none port=shift_ena
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Parameter indices
    const int S     = 0;
    const int S1    = 1;
    const int S11   = 2;
    const int S110  = 3;
    const int B0    = 4;
    const int B1    = 5;
    const int B2    = 6;
    const int B3    = 7;
    const int Count = 8;
    const int Wait  = 9;

    // Next‑state logic
    B3_next    = state[B2];
    S_next     = (state[S] && !d) || (state[S1] && !d) || (state[S110]&& !d) || (state[Wait]&&  ack);
    S1_next    = state[S] && d;
    Count_next = state[B3] || (state[Count] && !done_counting);
    Wait_next  = (state[Count] && done_counting) || (state[Wait]  && !ack);

    // Output logic
    done      = state[Wait];
    counting  = state[Count];
    shift_ena = state[B0] || state[B1] || state[B2] || state[B3];
}
