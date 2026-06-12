#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Phase 1: Fixed Test Sequence ---
    for (int i = 0; i < 10; i++) {
        // 'i % 4' gives values 0,1,2,3 which represent the 2-bit pattern {a, b}.
        int pattern = i % 4;
        bool a = (pattern & 0x2) != 0; // Most significant bit of the 2-bit pattern.
        bool b = (pattern & 0x1) != 0; // Least significant bit.
        bool out_dut;
        TopModule(a, b, out_dut);
        
        bool expected = !(a || b);
        
        if (out_dut != expected) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b 
                      << ", expected = " << expected 
                      << ", got = " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Sequence ---
    for (int i = 0; i < 200; i++) {
        bool a = (std::rand() % 2) ? true : false;
        bool b = (std::rand() % 2) ? true : false;
        bool out_dut;
        TopModule(a, b, out_dut);
        
        bool expected = !(a || b);
        
        if (out_dut != expected) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b 
                      << ", expected = " << expected 
                      << ", got = " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
	
    return 0;
}
