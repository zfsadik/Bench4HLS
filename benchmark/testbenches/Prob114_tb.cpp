#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset,
               bool predict_valid,
               bool predict_taken,
               ap_uint<32> &predict_history,
               bool train_mispredicted,
               bool train_taken,
               ap_uint<32> train_history);

void ref_model(bool reset,
               bool predict_valid,
               bool predict_taken,
               ap_uint<32> &predict_history_ref,
               bool train_mispredicted,
               bool train_taken,
               ap_uint<32> train_history,
               ap_uint<32> &reg_ref) {

    if (reset) {
        reg_ref = 0;
    }
    else if (train_mispredicted) {
        reg_ref = (train_history << 1) | (ap_uint<32>)(train_taken ? 1 : 0);
    }
    else if (predict_valid) {
        reg_ref = (reg_ref << 1) | (ap_uint<32>)(predict_taken ? 1 : 0);
    }
    predict_history_ref = reg_ref;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool reset;
    bool predict_valid;
    bool predict_taken;
    ap_uint<32> predict_history_dut = 0;
    bool train_mispredicted;
    bool train_taken;
    ap_uint<32> train_history;

    ap_uint<32> reg_ref = 0;
    ap_uint<32> predict_history_ref = 0;

    const int totalCycles = 5000;

    // Apply reset for 2 cycles before normal stimulus.
    for (int i = 0; i < 2; i++) {
        TopModule(true, false, false, predict_history_dut, false, false, 0);
        ref_model(true, false, false, predict_history_ref, false, false, 0, reg_ref);
    }

    for (int i = 0; i < totalCycles; i++) {
        reset = ((std::rand() % 32) == 0);
        predict_valid = ((std::rand() % 2) == 0);
        predict_taken = ((std::rand() & 1) != 0);
        train_mispredicted = ((std::rand() % 8) == 0);
        train_taken = ((std::rand() & 1) != 0);
        train_history = (((ap_uint<32>)(std::rand())) << 16) | (std::rand() & 0xFFFF);

        TopModule(reset, predict_valid, predict_taken, predict_history_dut,
                  train_mispredicted, train_taken, train_history);
        ref_model(reset, predict_valid, predict_taken, predict_history_ref,
                  train_mispredicted, train_taken, train_history, reg_ref);

        if (predict_history_dut != predict_history_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << predict_history_ref
                      << ", got " << predict_history_dut << std::endl;
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
