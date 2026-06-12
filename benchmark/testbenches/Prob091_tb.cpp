#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<4> in, 
               ap_uint<3> &out_both, 
               ap_uint<3> &out_any, 
               ap_uint<4> &out_different);

// Reference function: Computes expected outputs.
void ref_compute(ap_uint<4> in,
                 ap_uint<3> &ref_out_both,
                 ap_uint<3> &ref_out_any,
                 ap_uint<4> &ref_out_different) {
    ref_out_both[0] = in[0] & in[1];
    ref_out_both[1] = in[1] & in[2];
    ref_out_both[2] = in[2] & in[3];

    ref_out_any[0] = in[0] | in[1];
    ref_out_any[1] = in[1] | in[2];
    ref_out_any[2] = in[2] | in[3];

    ref_out_different[0] = in[0] ^ in[1];
    ref_out_different[1] = in[1] ^ in[2];
    ref_out_different[2] = in[2] ^ in[3];
    ref_out_different[3] = in[3] ^ in[0];
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;
    
    // DUT outputs.
    ap_uint<3> out_both_dut;
    ap_uint<3> out_any_dut;
    ap_uint<4> out_different_dut;
    
    // Reference outputs.
    ap_uint<3> out_both_ref;
    ap_uint<3> out_any_ref;
    ap_uint<4> out_different_ref;
    
    // Input vector.
    ap_uint<4> in_val;
    
    // --- Phase 1: Fixed Test Cases ---
    // Use a set of predetermined test values.
    const int fixedTests = 5;
    ap_uint<4> fixedVals[fixedTests] = { 3, 6, 12, 9, 5 };
    for (int i = 0; i < fixedTests; i++) {
        in_val = fixedVals[i];
        
        // Call DUT.
        TopModule(in_val, out_both_dut, out_any_dut, out_different_dut);
        // Compute reference outputs.
        ref_compute(in_val, out_both_ref, out_any_ref, out_different_ref);
        
        // Check each output.
        if (out_both_dut != out_both_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_both " << out_both_ref 
                      << ", got " << out_both_dut << std::endl;
            errorCount++;
        }
        if (out_any_dut != out_any_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_any " << out_any_ref 
                      << ", got " << out_any_dut << std::endl;
            errorCount++;
        }
        if (out_different_dut != out_different_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_different " << out_different_ref 
                      << ", got " << out_different_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Cases ---
    // Apply 100 random test cases.
    const int randomTests = 100;
    for (int i = 0; i < randomTests; i++) {
        // Generate a random 4-bit input.
        in_val = (std::rand() % 16);
        
        TopModule(in_val, out_both_dut, out_any_dut, out_different_dut);
        ref_compute(in_val, out_both_ref, out_any_ref, out_different_ref);
        
        if (out_both_dut != out_both_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_both " << out_both_ref 
                      << ", got " << out_both_dut << std::endl;
            errorCount++;
        }
        if (out_any_dut != out_any_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_any " << out_any_ref 
                      << ", got " << out_any_dut << std::endl;
            errorCount++;
        }
        if (out_different_dut != out_different_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected out_different " << out_different_ref 
                      << ", got " << out_different_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Print the test summary.
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
