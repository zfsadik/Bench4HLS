#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> a, ap_uint<8> b, ap_uint<8> c, ap_uint<8> d, ap_uint<8> &min);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCases = 5;   // Number of fixed test cases
    const int randomCases = 100; // Number of random test cases

    // --- Fixed Test Sequence ---
    struct TestVector {
        ap_uint<8> a, b, c, d;
    } fixedTests[] = {
        { 10, 20, 30, 40 },
        { 17,  2, 99, 50 },
        { 255, 100, 50, 200 },
        { 0,  0,  0,  0 },
        { 123, 123, 123, 124 }
    };

    for (int i = 0; i < fixedCases; i++) {
        ap_uint<8> a = fixedTests[i].a;
        ap_uint<8> b = fixedTests[i].b;
        ap_uint<8> c = fixedTests[i].c;
        ap_uint<8> d = fixedTests[i].d;
        ap_uint<8> dut_min = 0;
        TopModule(a, b, c, d, dut_min);

        ap_uint<8> expected = a;
        if (b < expected) expected = b;
        if (c < expected) expected = c;
        if (d < expected) expected = d;

        if (dut_min != expected) {
            std::cout << "Fixed test error at cycle " << cycleCount 
                      << ": a = " << a 
                      << ", b = " << b 
                      << ", c = " << c 
                      << ", d = " << d 
                      << ", expected min = " << expected 
                      << ", got min = " << dut_min << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < randomCases; i++) {
        ap_uint<8> a = std::rand() % 256;
        ap_uint<8> b = std::rand() % 256;
        ap_uint<8> c = std::rand() % 256;
        ap_uint<8> d = std::rand() % 256;
        ap_uint<8> dut_min = 0;
        TopModule(a, b, c, d, dut_min);

        ap_uint<8> expected = a;
        if (b < expected) expected = b;
        if (c < expected) expected = c;
        if (d < expected) expected = d;

        if (dut_min != expected) {
            std::cout << "Random test error at cycle " << cycleCount 
                      << ": a = " << a 
                      << ", b = " << b 
                      << ", c = " << c 
                      << ", d = " << d 
                      << ", expected min = " << expected 
                      << ", got min = " << dut_min << std::endl;
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
