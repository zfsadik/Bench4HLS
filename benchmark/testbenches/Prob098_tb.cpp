#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool d, bool &q);

// Reference function: Truth table derived from waveform (q = b OR c; a and d unused).
void ref_compute(bool a, bool b, bool c, bool d, bool &ref_q) {
    const bool truth_table[16] = {
        false, false, true,  true,
        true,  true,  true,  true,
        false, false, true,  true,
        true,  true,  true,  true
    };
    int idx = ((int)a << 3) | ((int)b << 2) | ((int)c << 1) | (int)d;
    ref_q = truth_table[idx];
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));
    
    int cycleCount = 0;
    int errorCount = 0;
    
    // DUT and reference outputs.
    bool q_dut = false;
    bool q_ref = false;
    
    // Input signals.
    bool a, b, c, d;
    unsigned int in_vec;
    
    // --- Phase 1: Fixed Test Cases ---
    // Test by enumerating the 16 possible combinations.
    for (in_vec = 0; in_vec < 16; in_vec++) {
        // Extract individual bits: 
        // a is the MSB, d is the LSB. (Here a = bit3, b = bit2, c = bit1, d = bit0)
        a = (in_vec >> 3) & 0x1;
        b = (in_vec >> 2) & 0x1;
        c = (in_vec >> 1) & 0x1;
        d = in_vec & 0x1;
        
        TopModule(a, b, c, d, q_dut);
        ref_compute(a, b, c, d, q_ref);
        
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << q_ref 
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Cases ---
    // Apply 100 random test cases.
    for (int i = 0; i < 100; i++) {
        // Generate a random 4-bit value.
        in_vec = std::rand() % 16;
        a = (in_vec >> 3) & 0x1;
        b = (in_vec >> 2) & 0x1;
        c = (in_vec >> 1) & 0x1;
        d = in_vec & 0x1;
        
        TopModule(a, b, c, d, q_dut);
        ref_compute(a, b, c, d, q_ref);
        
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
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
