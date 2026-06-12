#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<3> vec, ap_uint<3> &outv, bool &o2, bool &o1, bool &o0);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // --- Fixed Test Sequence (exhaustive: all 8 input combinations) ---
    for (int i = 0; i < 8; i++) {
        ap_uint<3> vec = i;
        ap_uint<3> dut_outv;
        bool o2_dut, o1_dut, o0_dut;
        TopModule(vec, dut_outv, o2_dut, o1_dut, o0_dut);

        // Expected outputs:
        ap_uint<3> expected_outv = vec;
        bool expected_o2 = vec[2];
        bool expected_o1 = vec[1];
        bool expected_o0 = vec[0];

        if (dut_outv != expected_outv) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected outv = " << expected_outv
                      << ", got outv = " << dut_outv << std::endl;
            errorCount++;
        }
        if (o2_dut != expected_o2) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o2 = " << expected_o2
                      << ", got o2 = " << o2_dut << std::endl;
            errorCount++;
        }
        if (o1_dut != expected_o1) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o1 = " << expected_o1
                      << ", got o1 = " << o1_dut << std::endl;
            errorCount++;
        }
        if (o0_dut != expected_o0) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o0 = " << expected_o0
                      << ", got o0 = " << o0_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Second Exhaustive Pass ---
    for (int i = 0; i < 8; i++) {
        ap_uint<3> vec = i;
        ap_uint<3> dut_outv;
        bool o2_dut, o1_dut, o0_dut;
        TopModule(vec, dut_outv, o2_dut, o1_dut, o0_dut);

        // Expected outputs:
        ap_uint<3> expected_outv = vec;
        bool expected_o2 = vec[2];
        bool expected_o1 = vec[1];
        bool expected_o0 = vec[0];

        if (dut_outv != expected_outv) {
            std::cout << "Second pass error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected outv = " << expected_outv
                      << ", got outv = " << dut_outv << std::endl;
            errorCount++;
        }
        if (o2_dut != expected_o2) {
            std::cout << "Second pass error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o2 = " << expected_o2
                      << ", got o2 = " << o2_dut << std::endl;
            errorCount++;
        }
        if (o1_dut != expected_o1) {
            std::cout << "Second pass error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o1 = " << expected_o1
                      << ", got o1 = " << o1_dut << std::endl;
            errorCount++;
        }
        if (o0_dut != expected_o0) {
            std::cout << "Second pass error at cycle " << cycleCount
                      << ": vec = " << vec
                      << ", expected o0 = " << expected_o0
                      << ", got o0 = " << o0_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
