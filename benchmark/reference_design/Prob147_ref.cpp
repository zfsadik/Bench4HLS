#include <ap_int.h>

void TopModule(
    bool        reset,
    bool        predict_valid,
    ap_uint<7>  predict_pc,
    bool       &predict_taken,
    ap_uint<7> &predict_history,
    bool        train_valid,
    bool        train_taken,
    bool        train_mispredicted,
    ap_uint<7>  train_history,
    ap_uint<7>  train_pc
) {
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=predict_valid
    #pragma HLS INTERFACE ap_none port=predict_pc
    #pragma HLS INTERFACE ap_none port=predict_taken
    #pragma HLS INTERFACE ap_none port=predict_history
    #pragma HLS INTERFACE ap_none port=train_valid
    #pragma HLS INTERFACE ap_none port=train_taken
    #pragma HLS INTERFACE ap_none port=train_mispredicted
    #pragma HLS INTERFACE ap_none port=train_history
    #pragma HLS INTERFACE ap_none port=train_pc
    #pragma HLS PIPELINE II=1

    // 2‑bit counter states
    const ap_uint<2> SNT = 0, LNT = 1, LT = 2, ST = 3;

    static ap_uint<2> pht[128] = {};
    #pragma HLS ARRAY_PARTITION variable=pht complete
    static ap_uint<7> history_r = 0;

    if (reset) {
        for (int i = 0; i < 128; i++) {
            #pragma HLS UNROLL
            pht[i] = LNT;
        }
        history_r = 0;
        predict_taken  = 0;
        predict_history = 0;
    } else {
        // --- Prediction ---
        if (predict_valid) {
            ap_uint<7> idx = history_r ^ predict_pc;
            ap_uint<2> cnt = pht[idx];
            predict_taken = cnt[1];
            predict_history = history_r;
            // update history
            history_r = (history_r << 1) | (cnt[1] ? 1 : 0);
        } else {
            predict_taken  = 0;
            predict_history = 0;
        }

        // --- Training ---
        if (train_valid) {
            ap_uint<7> tidx = train_history ^ train_pc;
            ap_uint<2> c = pht[tidx];
            if (train_taken && c < ST)        c++;
            else if (!train_taken && c > SNT) c--;
            pht[tidx] = c;
            if (train_mispredicted) {
                history_r = (train_history << 1) | (train_taken ? 1 : 0);
            }
        }
    }
}
