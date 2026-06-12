#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool &w, bool &x, bool &y, bool &z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 100; // Total number of test cycles

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Test loop: Generate random inputs and check expected outputs.
    for (int i = 0; i < totalCycles; i++) {
        // Generate random boolean values for a, b, and c.
        bool a = (std::rand() % 2) ? true : false;
        bool b = (std::rand() % 2) ? true : false;
        bool c = (std::rand() % 2) ? true : false;
        
        bool w_dut, x_dut, y_dut, z_dut;
        TopModule(a, b, c, w_dut, x_dut, y_dut, z_dut);

        bool expected_w = a;
        bool expected_x = b;
        bool expected_y = b;
        bool expected_z = c;

        // Check each output.
        if (w_dut != expected_w) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": a = " << a << " => expected w = " << expected_w 
                      << ", got w = " << w_dut << std::endl;
            errorCount++;
        }
        if (x_dut != expected_x) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": b = " << b << " => expected x = " << expected_x 
                      << ", got x = " << x_dut << std::endl;
            errorCount++;
        }
        if (y_dut != expected_y) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": b = " << b << " => expected y = " << expected_y 
                      << ", got y = " << y_dut << std::endl;
            errorCount++;
        }
        if (z_dut != expected_z) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": c = " << c << " => expected z = " << expected_z 
                      << ", got z = " << z_dut << std::endl;
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
