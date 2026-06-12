#include <iostream>
#include <cstdlib>
#include <ap_int.h>

// DUT Prototype
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
);

// Reference Model
void RefModel(
    bool        reset,
    bool        predict_valid,
    ap_uint<7>  predict_pc,
    bool       &predict_taken_ref,
    ap_uint<7> &predict_history_ref,
    bool        train_valid,
    bool        train_taken,
    bool        train_mispredicted,
    ap_uint<7>  train_history,
    ap_uint<7>  train_pc,
    ap_uint<2>  pht_ref[128],
    ap_uint<7>  &history_ref
) {
    const ap_uint<2> SNT = 0, LNT = 1, LT = 2, ST = 3;

    if (reset) {
        for (int i = 0; i < 128; i++) pht_ref[i] = LNT;
        history_ref = 0;
        predict_taken_ref  = 0;
        predict_history_ref = 0;
    } else {
        // Prediction
        if (predict_valid) {
            ap_uint<7> idx = history_ref ^ predict_pc;
            ap_uint<2> cnt = pht_ref[idx];
            predict_taken_ref = cnt[1];
            predict_history_ref = history_ref;
            history_ref = (history_ref << 1) | (cnt[1] ? 1 : 0);
        } else {
            predict_taken_ref  = 0;
            predict_history_ref = 0;
        }
        // Training
        if (train_valid) {
            ap_uint<7> tidx = train_history ^ train_pc;
            ap_uint<2> c = pht_ref[tidx];
            if (train_taken && c < ST)        c++;
            else if (!train_taken && c > SNT) c--;
            pht_ref[tidx] = c;
            if (train_mispredicted) {
                history_ref = (train_history << 1) | (train_taken ? 1 : 0);
            }
        }
    }
}

int main() {
    const int TOTAL = 2000;
    bool reset;
    bool predict_valid, train_valid;
    bool train_taken, train_mispredicted;
    ap_uint<7> predict_pc, train_history, train_pc;
    bool predict_taken, predict_taken_ref;
    ap_uint<7> predict_history, predict_history_ref;
    int cycleCount = 0, errorCount = 0;
    ap_uint<2> pht_ref[128] = {};
    ap_uint<7> history_ref = 0;

    std::srand(0);

    // Reset for 2 cycles
    for (int i = 0; i < 2; i++) {
        TopModule(true, false, 0, predict_taken, predict_history,
                  false, false, false, 0, 0);
        RefModel(true, false, 0, predict_taken_ref, predict_history_ref,
                 false, false, false, 0, 0, pht_ref, history_ref);
    }

    for (int i = 0; i < TOTAL; ++i) {
        reset             = ((std::rand() & 31) == 0);
        predict_valid      = (std::rand() & 3) == 0;
        predict_pc         = std::rand() & 0x7F;
        train_valid        = (std::rand() & 7) == 0;
        train_taken        = (std::rand() & 1);
        train_mispredicted = train_valid && ((std::rand() & 15) == 0);
        train_history      = std::rand() & 0x7F;
        train_pc           = std::rand() & 0x7F;

        TopModule(reset, predict_valid, predict_pc,
                  predict_taken, predict_history,
                  train_valid, train_taken, train_mispredicted,
                  train_history, train_pc);

        RefModel(reset, predict_valid, predict_pc,
                 predict_taken_ref, predict_history_ref,
                 train_valid, train_taken, train_mispredicted,
                 train_history, train_pc, pht_ref, history_ref);

        if (predict_taken != predict_taken_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected predict_taken=" << predict_taken_ref
                      << ", got=" << predict_taken << std::endl;
            errorCount++;
        }
        if (predict_history != predict_history_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected predict_history=" << predict_history_ref
                      << ", got=" << predict_history << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }
    return 0;
}
