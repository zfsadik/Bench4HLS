#include "ap_int.h"

void TopModule(bool reset,
               bool predict_valid,
               bool predict_taken,
               ap_uint<32> &predict_history,
               bool train_mispredicted,
               bool train_taken,
               ap_uint<32> train_history) {
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=predict_valid
#pragma HLS INTERFACE ap_none port=predict_taken
#pragma HLS INTERFACE ap_none port=predict_history
#pragma HLS INTERFACE ap_none port=train_mispredicted
#pragma HLS INTERFACE ap_none port=train_taken
#pragma HLS INTERFACE ap_none port=train_history
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<32> history_reg = 0;
#pragma HLS RESET variable=history_reg

    if (reset) {
        history_reg = 0;
    }
    else if (train_mispredicted) {
        history_reg = (train_history << 1) | (ap_uint<32>)(train_taken ? 1 : 0);
    }
    else if (predict_valid) {
        history_reg = (history_reg << 1) | (ap_uint<32>)(predict_taken ? 1 : 0);
    }

    predict_history = history_reg;
}
