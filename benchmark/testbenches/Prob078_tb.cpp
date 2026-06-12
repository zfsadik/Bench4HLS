#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1> p1a, ap_uint<1> p1b, ap_uint<1> p1c,
               ap_uint<1> p1d, ap_uint<1> p1e, ap_uint<1> p1f,
               ap_uint<1> p2a, ap_uint<1> p2b, ap_uint<1> p2c, ap_uint<1> p2d,
               ap_uint<1> &p1y, ap_uint<1> &p2y);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    
    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Variables for inputs and outputs.
    bool p1a, p1b, p1c, p1d, p1e, p1f;
    bool p2a, p2b, p2c, p2d;
    ap_uint<1> dut_p1y, dut_p2y;
    bool expected_p1y, expected_p2y;
    
    // --- Fixed Test Sequence ---
    std::cout << "Starting fixed test sequence..." << std::endl;
    for (int count = 0; count < 20; count++) {
        int r0 = count & 0x7;       // count[2:0]
        int r1 = (count >> 1) & 0x7;  // count[3:1]
        p1a = ((r0 >> 2) & 1) != 0;
        p1b = ((r0 >> 1) & 1) != 0;
        p1c = (r0 & 1) != 0;
        p1d = ((r1 >> 2) & 1) != 0;
        p1e = ((r1 >> 1) & 1) != 0;
        p1f = (r1 & 1) != 0;
        
        // For p2, use the 4-bit value of count.
        p2a = ((count >> 3) & 1) != 0;
        p2b = ((count >> 2) & 1) != 0;
        p2c = ((count >> 1) & 1) != 0;
        p2d = (count & 1) != 0;
        
        TopModule(p1a, p1b, p1c, p1d, p1e, p1f, p2a, p2b, p2c, p2d, dut_p1y, dut_p2y);
        
        expected_p1y = (p1a && p1b && p1c) || (p1d && p1e && p1f);
        expected_p2y = (p2a && p2b) || (p2c && p2d);
        
        if (dut_p1y != expected_p1y) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Fixed test inputs (p1a,p1b,p1c,p1d,p1e,p1f) = ("
                      << p1a << "," << p1b << "," << p1c << "," 
                      << p1d << "," << p1e << "," << p1f << ") "
                      << "expected p1y = " << expected_p1y 
                      << ", got p1y = " << dut_p1y << std::endl;
            errorCount++;
        }
        if (dut_p2y != expected_p2y) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Fixed test inputs (p2a,p2b,p2c,p2d) = ("
                      << p2a << "," << p2b << "," << p2c << "," << p2d << ") "
                      << "expected p2y = " << expected_p2y 
                      << ", got p2y = " << dut_p2y << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Random Test Sequence ---
    std::cout << "Starting random test sequence..." << std::endl;
    for (int i = 0; i < 400; i++) {
        // Randomize all 10 inputs.
        int rand_val = std::rand();
        p1a = (rand_val & 1) != 0;
        p1b = ((rand_val >> 1) & 1) != 0;
        p1c = ((rand_val >> 2) & 1) != 0;
        p1d = ((rand_val >> 3) & 1) != 0;
        p1e = ((rand_val >> 4) & 1) != 0;
        p1f = ((rand_val >> 5) & 1) != 0;
        
        int rand_val2 = std::rand();
        p2a = (rand_val2 & 1) != 0;
        p2b = ((rand_val2 >> 1) & 1) != 0;
        p2c = ((rand_val2 >> 2) & 1) != 0;
        p2d = ((rand_val2 >> 3) & 1) != 0;
        
        TopModule(p1a, p1b, p1c, p1d, p1e, p1f, p2a, p2b, p2c, p2d, dut_p1y, dut_p2y);
        
        expected_p1y = (p1a && p1b && p1c) || (p1d && p1e && p1f);
        expected_p2y = (p2a && p2b) || (p2c && p2d);
        
        if (dut_p1y != expected_p1y) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Random test inputs (p1a,p1b,p1c,p1d,p1e,p1f) = ("
                      << p1a << "," << p1b << "," << p1c << "," 
                      << p1d << "," << p1e << "," << p1f << ") "
                      << "expected p1y = " << expected_p1y 
                      << ", got p1y = " << dut_p1y << std::endl;
            errorCount++;
        }
        if (dut_p2y != expected_p2y) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Random test inputs (p2a,p2b,p2c,p2d) = ("
                      << p2a << "," << p2b << "," << p2c << "," << p2d << ") "
                      << "expected p2y = " << expected_p2y 
                      << ", got p2y = " << dut_p2y << std::endl;
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
