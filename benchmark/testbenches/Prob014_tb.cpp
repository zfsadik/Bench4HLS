#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // --- Phase 1: Fixed Test Sequence (a,b) = (0,0), (0,1), (1,0), (1,1) ---
    for (int count = 0; count < 10; count++) {
        
        int pattern = count % 4;
        bool a = (pattern >> 1) & 1;  // Extracts the higher bit.
        bool b = pattern & 1;         // Extracts the lower bit.
        bool out_dut;
        TopModule(a, b, out_dut);

        bool expected = a && b;
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
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < 200; i++) {
        bool a = (std::rand() % 2) ? true : false;
        bool b = (std::rand() % 2) ? true : false;
        bool out_dut;
        TopModule(a, b, out_dut);

        bool expected = a && b;
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
