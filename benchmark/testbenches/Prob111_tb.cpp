#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool load, bool ena, ap_uint<2> amount, ap_uint<64> data, ap_uint<64> &q);

// Reference model for the shift register.
void ref_model(bool load, bool ena, ap_uint<2> amount, ap_uint<64> data, ap_uint<64> &q_ref, ap_uint<64> &reg_ref) {
    if (load) {
        reg_ref = data;
    } else if (ena) {
        if (amount == 0x0) {
            reg_ref = reg_ref << 1;  // shift left by 1
        } else if (amount == 0x1) {
            reg_ref = reg_ref << 8;  // shift left by 8
        } else if (amount == 0x2) {
            // Arithmetic shift right by 1: explicit MSB replication.
            ap_uint<1> msb = reg_ref[63];
            reg_ref = (reg_ref >> 1) | ((ap_uint<64>)msb << 63);
        } else if (amount == 0x3) {
            // Arithmetic shift right by 8: explicit MSB replication.
            ap_uint<1> msb = reg_ref[63];
            ap_uint<8> sign_ext = msb ? (ap_uint<8>)0xFF : (ap_uint<8>)0x00;
            reg_ref = (reg_ref >> 8) | ((ap_uint<64>)sign_ext << 56);
        }
    }
    q_ref = reg_ref;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // DUT signals.
    bool load;
    bool ena;
    ap_uint<2> amount;
    ap_uint<64> data;
    ap_uint<64> q_dut = 0;

    // Reference signals and internal state.
    ap_uint<64> q_ref = 0;
    ap_uint<64> reg_ref = 0;

    // Total number of cycles.
    const int totalCycles = 5000;

    for (int i = 0; i < totalCycles; i++) {
        // Generate stimulus.
        load = ((std::rand() % 32) == 0);  // load asserted with probability 1/32.
        ena = (std::rand() % 2) ? true : false;
        amount = std::rand() & 0x3;  // random 2-bit value.
        // Generate a 64-bit random data value: we combine two 32-bit random numbers.
        data = ((ap_uint<64>)(std::rand()) << 32) | (std::rand());

        TopModule(load, ena, amount, data, q_dut);
        ref_model(load, ena, amount, data, q_ref, reg_ref);

        // Compare DUT output to reference.
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final summary.
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
