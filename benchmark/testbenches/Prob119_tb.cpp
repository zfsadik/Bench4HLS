#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool do_sub, ap_uint<8> a, ap_uint<8> b, ap_uint<8> &out, bool &result_is_zero);

void ref_compute(bool do_sub, ap_uint<8> a, ap_uint<8> b, ap_uint<8> &ref_out, bool &ref_zero) {
    if (!do_sub) {
        ref_out = a + b;
    } else {
        ref_out = a - b;
    }
    ref_zero = (ref_out == 0);
}

int main() {
    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));
    
    int cycleCount = 0;
    int errorCount = 0;
    
    // DUT signals.
    bool do_sub;
    ap_uint<8> a, b;
    ap_uint<8> out_dut = 0;
    bool result_is_zero_dut = false;
    
    // Reference signals.
    ap_uint<8> out_ref = 0;
    bool result_is_zero_ref = false;
    
    // --- Phase 1: Fixed Test Cases ---
    const int fixedTests = 8;
    ap_uint<8> fixed_a[fixedTests] = { 10, 0, 255, 100, 50, 20, 0,  200 };
    ap_uint<8> fixed_b[fixedTests] = {  5, 0,   1, 155, 50, 20, 1,   55 };
    bool fixed_do_sub[fixedTests] = { false, true, false, true, false, true, false, true };
    
    for (int i = 0; i < fixedTests; i++) {
        do_sub = fixed_do_sub[i];
        a = fixed_a[i];
        b = fixed_b[i];
        
        TopModule(do_sub, a, b, out_dut, result_is_zero_dut);
        ref_compute(do_sub, a, b, out_ref, result_is_zero_ref);
        
        if ( (out_dut != out_ref) || (result_is_zero_dut != result_is_zero_ref) ) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected (out=" << out_ref 
                      << ", zero=" << result_is_zero_ref 
                      << "), got (out=" << out_dut 
                      << ", zero=" << result_is_zero_dut << ")" << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Cases ---
    const int numRandom = 100;
    for (int i = 0; i < numRandom; i++) {
        do_sub = (std::rand() & 1);
        a = std::rand() % 256;  // 8-bit value
        b = std::rand() % 256;
        
        TopModule(do_sub, a, b, out_dut, result_is_zero_dut);
        ref_compute(do_sub, a, b, out_ref, result_is_zero_ref);
        
        if ( (out_dut != out_ref) || (result_is_zero_dut != result_is_zero_ref) ) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected (out=" << out_ref 
                      << ", zero=" << result_is_zero_ref 
                      << "), got (out=" << out_dut 
                      << ", zero=" << result_is_zero_dut << ")" << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Final test summary.
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
