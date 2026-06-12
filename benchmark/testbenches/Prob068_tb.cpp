#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> in, ap_uint<3> &pos);

int main() {
    std::srand(std::time(nullptr));
    int errorCount = 0;
    int cycleCount = 0;

    // Exhaustive test: iterate through all 256 possible 8-bit input values.
    for (int i = 0; i < 256; i++) {
        ap_uint<8> in = i;
        ap_uint<3> dut_pos;
        TopModule(in, dut_pos);

        // Compute the expected output: find the index of the least-significant '1'
        unsigned int expected = 0;
        bool found = false;
        for (int j = 0; j < 8; j++) {
            if (!found && ((in >> j) & 1)) {
                expected = j;
                found = true;
            }
        }
        // If in==0, expected remains 0.
        cycleCount++;
        if (dut_pos != expected) {
            std::cout << "Error at cycle " << cycleCount 
                      << ": Input = " << (unsigned int)in 
                      << " | DUT pos = " << (unsigned int)dut_pos 
                      << " | Expected pos = " << expected << std::endl;
            errorCount++;
        }
    }

    // Additional 100 random test cases.
    for (int i = 0; i < 100; i++) {
        int r = std::rand() % 256;
        ap_uint<8> in = r;
        ap_uint<3> dut_pos;
        TopModule(in, dut_pos);

        unsigned int expected = 0;
        bool found = false;
        for (int j = 0; j < 8; j++) {
            if (!found && ((in >> j) & 1)) {
                expected = j;
                found = true;
            }
        }
        cycleCount++;
        if (dut_pos != expected) {
            std::cout << "Error at cycle " << cycleCount 
                      << ": Input = " << (unsigned int)in 
                      << " | DUT pos = " << (unsigned int)dut_pos 
                      << " | Expected pos = " << expected << std::endl;
            errorCount++;
        }
    }

    // Report test summary in decimal.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
