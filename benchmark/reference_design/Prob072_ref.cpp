#include "ap_int.h"

void TopModule(bool areset, bool train_valid, bool train_taken, ap_uint<2> &state) {
    #pragma HLS INTERFACE ap_none port=areset
    #pragma HLS INTERFACE ap_none port=train_valid
    #pragma HLS INTERFACE ap_none port=train_taken
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<2> state_reg = 1; // Synchronous reset sets counter to 1.

    if (areset) {
        state_reg = 1;
    } else if (train_valid) {
        if (train_taken && state_reg < 3)
            state_reg = state_reg + 1;
        else if (!train_taken && state_reg > 0)
            state_reg = state_reg - 1;
    }
    state = state_reg;
}
