#include "ap_int.h"

// States: START=8, B0..B7=0..7, STOP=9, DONE=10, ERR=11
void TopModule(bool in, bool reset, bool &done) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    enum { B0=0, B1=1, B2=2, B3=3, B4=4, B5=5, B6=6, B7=7,
           START=8, STOP=9, DONE=10, ERR=11 };

    static ap_uint<4> state = START;
    #pragma HLS RESET variable=state
    ap_uint<4> next;

    // Combinational next‑state logic
    switch (state) {
      case START: next = in ? START : B0; break;
      case B0:    next = B1;               break;
      case B1:    next = B2;               break;
      case B2:    next = B3;               break;
      case B3:    next = B4;               break;
      case B4:    next = B5;               break;
      case B5:    next = B6;               break;
      case B6:    next = B7;               break;
      case B7:    next = STOP;             break;
      case STOP:  next = in ? DONE : ERR;  break;
      case DONE:  next = START;            break;
      case ERR:   next = in ? START : ERR; break;
      default:    next = START;            break;
    }

    done = (state == DONE);

    if (reset) state = START;
    else        state = next;
}
