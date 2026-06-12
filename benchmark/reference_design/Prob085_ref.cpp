#include "ap_int.h"

void TopModule(bool reset, bool x, bool &z) {
	#pragma HLS INTERFACE ap_none port=z
    #pragma HLS INTERFACE ap_none port=x
    #pragma HLS INTERFACE ap_none port=reset
	#pragma HLS INTERFACE ap_ctrl_none port=return
	#pragma HLS PIPELINE II=1

  // One-hot encoding for two states: state A = 1, state B = 2.
  static ap_uint<2> state = 1;

  ap_uint<2> cur_state = state;

  if (reset) {
    state = 1; // Force state A.
  }
  else {
    if (cur_state == 1) {
      // In state A.
      if (x)
        state = 2;  // Transition to B when x==1.
      else
        state = 1;  // Remain in A when x==0.
    }
    else {
      // In state B, remain in B.
      state = 2;
    }
  }

  if ((cur_state == 1 && x == true) || (cur_state == 2 && x == false))
    z = true;
  else
    z = false;
}
