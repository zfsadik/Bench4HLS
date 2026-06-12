#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<1> x, ap_uint<3> y, ap_uint<1> &Y0, ap_uint<1> &z);

void ref_model(ap_uint<1> x, ap_uint<3> y, ap_uint<1> &Y0_ref, ap_uint<1> &z_ref) {
    // Direct encoding of state-assigned table: Y0_table[present_state][x]
    bool Y0_table[5][2] = {
        {false, true},  // y=000: x=0->000(Y0=0), x=1->001(Y0=1)
        {true,  false}, // y=001: x=0->001(Y0=1), x=1->100(Y0=0)
        {false, true},  // y=010: x=0->010(Y0=0), x=1->001(Y0=1)
        {true,  false}, // y=011: x=0->001(Y0=1), x=1->010(Y0=0)
        {true,  false}  // y=100: x=0->011(Y0=1), x=1->100(Y0=0)
    };
    bool z_table[5] = {false, false, false, true, true};

    int yi = (int)(unsigned)y;
    if (yi <= 4) {
        Y0_ref = Y0_table[yi][(int)x];
        z_ref  = z_table[yi];
    } else {
        Y0_ref = 0;
        z_ref  = 0;
    }
}

int main() {
    int errorCount = 0, cycleCount = 0;
    ap_uint<1> x, Y0_dut, z_dut, Y0_ref, z_ref;
    ap_uint<3> y;

    // Exhaustive test: all y ∈ [0..7], x ∈ {0,1}
    for (int yi = 0; yi < 8; yi++) {
        for (int xi = 0; xi < 2; xi++) {
            y = yi; x = xi;
            TopModule(x, y, Y0_dut, z_dut);
            ref_model(x, y, Y0_ref, z_ref);

            if (Y0_dut != Y0_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected Y0=" << Y0_ref
                          << ", got " << Y0_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
            if (z_dut != z_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected z=" << z_ref
                          << ", got " << z_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Randomized testing: 100 cases
    for (int i = 0; i < 100; i++) {
        y = rand() & 7;
        x = rand() & 1;
        TopModule(x, y, Y0_dut, z_dut);
        ref_model(x, y, Y0_ref, z_ref);

        if (Y0_dut != Y0_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected Y0=" << Y0_ref
                      << ", got " << Y0_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
        if (z_dut != z_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected z=" << z_ref
                      << ", got " << z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Final summary
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
