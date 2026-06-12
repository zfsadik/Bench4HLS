#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool &q);

void ref_compute(bool a, bool b, bool &q) {
    q = !(!a || !b);
}

int main() {
    // Seed random generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    bool a = false;
    bool b = false;
    bool q_dut = false;
    bool q_ref = false;

    // --- Phase 1: Fixed Test Cases ---
    // Cycle through all 2-bit combinations. There are 4 possible combinations.
    for (int i = 0; i < 4; i++) {
        // Extract the two bits: MSB is a, LSB is b.
        a = (i >> 1) & 0x1;
        b = (i >> 0) & 0x1;
        
        // Call the DUT.
        TopModule(a, b, q_dut);
        // Compute the reference output.
        ref_compute(a, b, q_ref);
        
        // Check results.
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
        // Generate random values for a and b (0 or 1).
        a = (std::rand() & 1) ? true : false;
        b = (std::rand() & 1) ? true : false;
        
        // Call the DUT.
        TopModule(a, b, q_dut);
        // Compute the expected output.
        ref_compute(a, b, q_ref);
        
        // Check for mismatch.
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << q_ref 
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Print summary report.
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
