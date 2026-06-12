#include "ap_int.h"

void TopModule(bool reset, bool in, bool &disc, bool &flag, bool &err) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=disc
    #pragma HLS INTERFACE ap_none port=flag
    #pragma HLS INTERFACE ap_none port=err
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // States: S0=0, S1=1, S2=2, S3=3, S4=4, S5=5, S6=6, SERR=7, SDISC=8, SFLAG=9
    static ap_uint<4> state = 0;
    #pragma HLS RESET variable=state
    ap_uint<4> next;

    disc  = (state == 8);
    flag  = (state == 9);
    err   = (state == 7);

    if (reset) {
        state = 0;
    } else {
        // Next-state logic
        switch (state) {
          case 0:  next = in ? 1 : 0;   break; // S0
          case 1:  next = in ? 2 : 0;   break; // S1
          case 2:  next = in ? 3 : 0;   break; // S2
          case 3:  next = in ? 4 : 0;   break; // S3
          case 4:  next = in ? 5 : 0;   break; // S4
          case 5:  next = in ? 6 : 8;   break; // S5→S6 or SDISC
          case 6:  next = in ? 7 : 9;   break; // S6→SERR or SFLAG
          case 7:  next = in ? 7 : 0;   break; // SERR
          case 9:  next = in ? 1 : 0;   break; // SFLAG
          case 8:  next = in ? 1 : 0;   break; // SDISC
          default: next = 0;            break;
        }
        state = next;
    }
}
