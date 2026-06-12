#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool p1a, bool p1b, bool p1c, bool p1d, bool &p1y,
               bool p2a, bool p2b, bool p2c, bool p2d, bool &p2y);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int numFixedTests = 16;   // Number of fixed test cycles
    const int numRandomTests = 100;   // Number of random test cycles

    // --- Fixed Test Sequence ---
    for (int i = 0; i < numFixedTests; i++) {
        bool p1a = ((i >> 3) & 1) ? true : false;
        bool p1b = ((i >> 2) & 1) ? true : false;
        bool p1c = ((i >> 1) & 1) ? true : false;
        bool p1d = (i & 1) ? true : false;
        bool p2a = (((i + 1) >> 3) & 1) ? true : false;
        bool p2b = (((i + 1) >> 2) & 1) ? true : false;
        bool p2c = (((i + 1) >> 1) & 1) ? true : false;
        bool p2d = ((i + 1) & 1) ? true : false;

        bool dut_p1y, dut_p2y;
        TopModule(p1a, p1b, p1c, p1d, dut_p1y,
                  p2a, p2b, p2c, p2d, dut_p2y);

        // Compute expected outputs.
        bool expected_p1y = !(p1a && p1b && p1c && p1d);
        bool expected_p2y = !(p2a && p2b && p2c && p2d);

        if (dut_p1y != expected_p1y) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": p1a=" << (int)p1a 
                      << ", p1b=" << (int)p1b 
                      << ", p1c=" << (int)p1c 
                      << ", p1d=" << (int)p1d 
                      << ", expected p1y=" << (int)expected_p1y 
                      << ", got p1y=" << (int)dut_p1y << std::endl;
            errorCount++;
        }
        if (dut_p2y != expected_p2y) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": p2a=" << (int)p2a 
                      << ", p2b=" << (int)p2b 
                      << ", p2c=" << (int)p2c 
                      << ", p2d=" << (int)p2d 
                      << ", expected p2y=" << (int)expected_p2y 
                      << ", got p2y=" << (int)dut_p2y << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < numRandomTests; i++) {
        // Randomly generate boolean values for each input.
        bool p1a = (std::rand() % 2) ? true : false;
        bool p1b = (std::rand() % 2) ? true : false;
        bool p1c = (std::rand() % 2) ? true : false;
        bool p1d = (std::rand() % 2) ? true : false;
        bool p2a = (std::rand() % 2) ? true : false;
        bool p2b = (std::rand() % 2) ? true : false;
        bool p2c = (std::rand() % 2) ? true : false;
        bool p2d = (std::rand() % 2) ? true : false;

        bool dut_p1y, dut_p2y;
        TopModule(p1a, p1b, p1c, p1d, dut_p1y,
                  p2a, p2b, p2c, p2d, dut_p2y);

        // Expected outputs.
        bool expected_p1y = !(p1a && p1b && p1c && p1d);
        bool expected_p2y = !(p2a && p2b && p2c && p2d);

        if (dut_p1y != expected_p1y) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": p1a=" << (int)p1a 
                      << ", p1b=" << (int)p1b 
                      << ", p1c=" << (int)p1c 
                      << ", p1d=" << (int)p1d 
                      << ", expected p1y=" << (int)expected_p1y 
                      << ", got p1y=" << (int)dut_p1y << std::endl;
            errorCount++;
        }
        if (dut_p2y != expected_p2y) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": p2a=" << (int)p2a 
                      << ", p2b=" << (int)p2b 
                      << ", p2c=" << (int)p2c 
                      << ", p2d=" << (int)p2d 
                      << ", expected p2y=" << (int)expected_p2y 
                      << ", got p2y=" << (int)dut_p2y << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
