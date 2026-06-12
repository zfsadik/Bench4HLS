#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool x3, bool x2, bool x1, bool &f);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    
    // Reference model for the logic function.
    // f_expected = ((!x3) && x2) || (x3 && x1)
    for (int i = 0; i < 8; i++) {
        bool x3 = (i >> 2) & 1;
        bool x2 = (i >> 1) & 1;
        bool x1 = i & 1;
        bool f_dut;
        TopModule(x3, x2, x1, f_dut);
        bool f_expected = ((!x3) && x2) || (x3 && x1);
        if (f_dut != f_expected) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (x3, x2, x1) = (" << (int)x3 << ", " << (int)x2 << ", " << (int)x1 
                      << ") expected f = " << (int)f_expected 
                      << ", got f = " << (int)f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Random test sequence.
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const int randomCycles = 100;
    for (int i = 0; i < randomCycles; i++) {
        bool x3 = std::rand() % 2;
        bool x2 = std::rand() % 2;
        bool x1 = std::rand() % 2;
        bool f_dut;
        TopModule(x3, x2, x1, f_dut);
        bool f_expected = ((!x3) && x2) || (x3 && x1);
        if (f_dut != f_expected) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (x3, x2, x1) = (" << (int)x3 << ", " << (int)x2 << ", " << (int)x1 
                      << ") expected f = " << (int)f_expected 
                      << ", got f = " << (int)f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Final summary report.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " 
                  << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
