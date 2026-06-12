#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool load, ap_uint<2> ena, ap_uint<100> data, ap_uint<100> &q);

void ref_compute(bool load, ap_uint<2> ena, ap_uint<100> data, ap_uint<100> &q_ref, ap_uint<100> &reg_ref) {
    if (load) {
        reg_ref = data;
    } else if (ena == 1) {
        // Right rotate by 1: LSB wraps to MSB
        bool lsb = reg_ref[0];
        reg_ref >>= 1;
        reg_ref[99] = lsb;
    } else if (ena == 2) {
        // Left rotate by 1: MSB wraps to LSB
        bool msb = reg_ref[99];
        reg_ref <<= 1;
        reg_ref[0] = msb;
    }
    q_ref = reg_ref;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<100> q_dut = 0;
    ap_uint<100> q_ref = 0;

    bool load;
    ap_uint<2> ena;
    ap_uint<100> data;

    ap_uint<100> reg_ref = 0;

    const int totalCycles = 1000;

    // --- Stimulus Generation & Testing ---
    for (int i = 0; i < totalCycles; i++) {
        // Generate random stimulus:
        load = ((std::rand() % 32) == 0);
        ena = std::rand() % 4;       // 0 to 3
        // Generate a random 100-bit data value.
        data = 0;
        for (int bit = 0; bit < 100; bit++) {
            bool rbit = ((std::rand() & 1) != 0);
            data[bit] = rbit;
        }

        TopModule(load, ena, data, q_dut);
        ref_compute(load, ena, data, q_ref, reg_ref);

        // Compare DUT output with reference.
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print test summary.
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
