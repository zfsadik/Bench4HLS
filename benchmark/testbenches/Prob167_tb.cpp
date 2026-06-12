#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

// DUT prototype
void TopModule(ap_int<32> in[8], ap_int<32> out[8], bool &valid);

// Reference sort in C++
void ref_sort(const int in[8], int out_ref[8], bool &valid_ref) {
    for (int i = 0; i < 8; i++) 
		out_ref[i] = in[i];
    for (int i = 0; i < 7; i++) {
        for (int j = i+1; j < 8; j++) {
            if (out_ref[i] > out_ref[j]) {
                int tmp = out_ref[i];
                out_ref[i] = out_ref[j];
                out_ref[j] = tmp;
            }
        }
    }
    valid_ref = true;
}

int main() {
    std::srand((unsigned)std::time(nullptr));
    int cycleCount = 0, errorCount = 0;

    ap_int<32> in_dut[8], out_dut[8];
    int        in_ref[8], out_ref[8];
    bool       valid_dut, valid_ref;

    // --- Phase 1: Fixed Test Vector ---
    const int A[8]       = { 22, 5, -9, 3, -17, 38, 0, 11 };
    const int outData[8] = { -17, -9, 0, 3, 5, 11, 22, 38 };

    for (int i = 0; i < 8; i++) {
        in_dut[i] = A[i];
        in_ref[i] = A[i];
    }
    TopModule(in_dut, out_dut, valid_dut);
    ref_sort(in_ref, out_ref, valid_ref);

    // Compare fixed results
    for (int i = 0; i < 8; i++) {
        if ((int)out_dut[i] != out_ref[i]) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out[" << i << "] = "
                      << out_ref[i] << ", got " << out_dut[i]
                      << std::endl;
            errorCount++;
        }
    }
    if (valid_dut != valid_ref) {
        std::cout << "Mismatch at cycle " << cycleCount 
                  << ": expected valid = " << valid_ref 
                  << ", got " << valid_dut << std::endl;
        errorCount++;
    }
    cycleCount++;

    // --- Phase 2: Random Tests (500 cases) ---
    const int numRandom = 500;
    for (int t = 0; t < numRandom; t++) {
        // Generate random inputs
        for (int i = 0; i < 8; i++) {
            int v = (std::rand() % 200) - 100;  // range [-100,99]
            in_dut[i] = v; in_ref[i] = v;
        }

        TopModule(in_dut, out_dut, valid_dut);
        ref_sort(in_ref, out_ref, valid_ref);

        // Compare DUT vs reference
        for (int i = 0; i < 8; i++) {
            if ((int)out_dut[i] != out_ref[i]) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected out[" << i << "] = "
                          << out_ref[i] << ", got " << out_dut[i]
                          << std::endl;
                errorCount++;
            }
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected valid = " << valid_ref 
                      << ", got " << valid_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
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
