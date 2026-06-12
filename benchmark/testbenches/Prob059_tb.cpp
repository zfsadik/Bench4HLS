#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<8> a, ap_uint<8> b, bool sel, ap_uint<8> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int numFixedTests = 6;   // Number of fixed test cases
    const int numRandomTests = 100; // Number of random test cases

    // Fixed test cases (predefined):
    struct TestCase {
        ap_uint<8> a;
        ap_uint<8> b;
        bool sel;
        ap_uint<8> expected;
    };

    TestCase fixedTests[] = {
        {170, 187, false, 187}, // 170,187, false => out = b = 187
        {170, 187, true,  170}, // 170,187, true  => out = a = 170
        {255, 0,   false, 0},   // 255,0, false   => out = 0
        {255, 0,   true,  255}, // 255,0, true    => out = 255
        {18, 52,   false, 52},  // 18,52, false   => out = 52
        {18, 52,   true,  18}   // 18,52, true    => out = 18
    };

    for (int i = 0; i < numFixedTests; i++) {
        ap_uint<8> a = fixedTests[i].a;
        ap_uint<8> b = fixedTests[i].b;
        bool sel = fixedTests[i].sel;
        ap_uint<8> dut_out;
        TopModule(a, b, sel, dut_out);

        if (dut_out != fixedTests[i].expected) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": a = " << (unsigned int)a 
                      << ", b = " << (unsigned int)b 
                      << ", sel = " << sel 
                      << ", expected out = " << (unsigned int)fixedTests[i].expected 
                      << ", got out = " << (unsigned int)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Random test sequence.
    std::srand(42);
    for (int i = 0; i < numRandomTests; i++) {
        ap_uint<8> a = std::rand() % 256;
        ap_uint<8> b = std::rand() % 256;
        bool sel = (std::rand() % 2) ? true : false;
        ap_uint<8> dut_out;
        TopModule(a, b, sel, dut_out);

        ap_uint<8> expected = sel ? a : b;
        if (dut_out != expected) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": a = " << (unsigned int)a 
                      << ", b = " << (unsigned int)b 
                      << ", sel = " << sel 
                      << ", expected out = " << (unsigned int)expected 
                      << ", got out = " << (unsigned int)dut_out << std::endl;
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

