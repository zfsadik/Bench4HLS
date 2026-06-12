#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<100> in, 
               ap_uint<99> &out_both, 
               ap_uint<99> &out_any, 
               ap_uint<100> &out_different);

// Reference function: Computes the expected outputs.
void ref_compute(ap_uint<100> in,
                 ap_uint<99> &ref_out_both,
                 ap_uint<99> &ref_out_any,
                 ap_uint<100> &ref_out_different) {
    for (int i = 0; i < 99; i++) {
        ref_out_both[i] = in[i] & in[i+1];
        ref_out_any[i]  = in[i] | in[i+1];
        ref_out_different[i] = in[i] ^ in[i+1];
    }
    ref_out_different[99] = in[99] ^ in[0];
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;
    
    ap_uint<99> out_both_dut;
    ap_uint<99> out_any_dut;
    ap_uint<100> out_different_dut;
    
    ap_uint<99> out_both_ref;
    ap_uint<99> out_any_ref;
    ap_uint<100> out_different_ref;
    
    // Input vector.
    ap_uint<100> in_val;
    
    // Number of test cases.
    const int totalTests = 100;
    
    // Run test cases.
    for (int i = 0; i < totalTests; i++) {
        // Generate a random 100-bit number.
        in_val = 0;
        // Since 100 bits do not fit in 64-bit, generate bit-by-bit.
        for (int bit = 0; bit < 100; bit++) {
            bool rbit = (std::rand() & 1) ? true : false;
            in_val[bit] = rbit;
        }
        
        // Compute reference outputs.
        ref_compute(in_val, out_both_ref, out_any_ref, out_different_ref);
        
        TopModule(in_val, out_both_dut, out_any_dut, out_different_dut);
        
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
