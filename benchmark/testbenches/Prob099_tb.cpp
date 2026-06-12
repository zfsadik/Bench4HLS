#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool c, bool d, bool &q);

// Reference function that computes q = (a OR b) AND (c OR d)
void ref_compute(bool a, bool b, bool c, bool d, bool &q_ref) {
    static const bool truth_table[16] = {
        0,0,0,0, 0,1,1,1, 0,1,1,1, 0,1,1,1
    };
    int idx = (a << 3) | (b << 2) | (c << 1) | d;
    q_ref = truth_table[idx];
}

int main() {
    // Seed random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;
    
    bool a, b, c, d;
    bool q_dut = false;
    bool q_ref = false;
    
    // --- Phase 1: Fixed Test Cases ---
    // Test all 16 combinations of a, b, c, d.
    for (int in_val = 0; in_val < 16; in_val++) {
        // Map in_val bits to inputs (choose a = bit3, b = bit2, c = bit1, d = bit0)
        a = (in_val >> 3) & 0x1;
        b = (in_val >> 2) & 0x1;
        c = (in_val >> 1) & 0x1;
        d = in_val & 0x1;
        
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
    // Run 100 random tests.
    for (int i = 0; i < 100; i++) {
        a = ((std::rand() & 1) != 0);
        b = ((std::rand() & 1) != 0);
        c = ((std::rand() & 1) != 0);
        d = ((std::rand() & 1) != 0);
        
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
        std::cout << "Test Passed: All " << cycleCount 
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount 
                  << " cases." << std::endl;
    }
    
    return 0;
}
