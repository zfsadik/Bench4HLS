#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool sel, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Phase 1: Fixed Test Sequence
    int fixedTests[] = { 0b000, 0b001, 0b010, 0b011, 0b100, 0b101, 0b110, 0b111 };
    int numFixedTests = sizeof(fixedTests) / sizeof(fixedTests[0]);
    for (int i = 0; i < numFixedTests; i++) {
        int pattern = fixedTests[i];
        bool a   = (pattern >> 2) & 1; // Extract bit2.
        bool b   = (pattern >> 1) & 1; // Extract bit1.
        bool sel = pattern & 1;        // Extract bit0.
        bool dutOut = false;
        
		TopModule(a, b, sel, dutOut);
        bool expected = sel ? b : a;
        if (dutOut != expected) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", sel = " << sel << ", expected = " << expected 
                      << ", got = " << dutOut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Phase 2: Random Test Sequence
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const int numRandomTests = 100;
    for (int i = 0; i < numRandomTests; i++) {
        bool a = (std::rand() % 2);
        bool b = (std::rand() % 2);
        bool sel = (std::rand() % 2);
        bool dutOut = false;
        
		TopModule(a, b, sel, dutOut);
        bool expected = sel ? b : a;
        if (dutOut != expected) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": a = " << a << ", b = " << b 
                      << ", sel = " << sel << ", expected = " << expected 
                      << ", got = " << dutOut << std::endl;
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
