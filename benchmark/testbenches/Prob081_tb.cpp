#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool enable, bool S, bool A, bool B, bool C, bool &Z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    ap_uint<8> golden = 0;  // initial value of the shift register

    bool dut_Z;
    bool expected_Z;

    // Input variables.
    bool enable;
    bool S;
    bool A, B, C;
    ap_uint<3> addr;

    // --- Fixed Test Sequence ---
    std::cout << "Starting fixed test sequence..." << std::endl;

    // Step 1: Initialize inputs.
    enable = false;
    A = false; B = false; C = false;  // Address 0
    S = false;  // S is don't care when shifting is disabled
    // Simulate one cycle (assume clk = true).
    TopModule(enable, S, A, B, C, dut_Z);
    // Golden model remains unchanged.
    addr = 0;  // address 0
    expected_Z = (golden >> addr) & 1;
    if (dut_Z != expected_Z) {
        std::cout << "Error at test case " << cycleCount << ": initial fixed test, expected Z=" << expected_Z << ", got Z=" << dut_Z << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Step 2: Shift in a known pattern.
    // Enable shifting and shift in S = 1 for one cycle.
    enable = true;
    S = true;
    TopModule(enable, S, A, B, C, dut_Z);
    golden = ((golden & 0x7F) << 1) | S;
    cycleCount++;

    // Then shift in S = 0 for 7 cycles.
    S = false;
    for (int i = 0; i < 7; i++) {
        TopModule(enable, S, A, B, C, dut_Z);
        golden = ((golden & 0x7F) << 1) | S;
        cycleCount++;
    }

    // Step 3: Disable shifting and test the multiplexer.
    enable = false;
    // Test with addresses: 5, 6, 7, 0, 1.
    int fixedAddr[5] = {5, 6, 7, 0, 1};
    for (int i = 0; i < 5; i++) {
        ap_uint<3> testAddr = fixedAddr[i];
        A = testAddr[2];
        B = testAddr[1];
        C = testAddr[0];
        TopModule(enable, S, A, B, C, dut_Z);
        expected_Z = (golden >> testAddr) & 1;
        if (dut_Z != expected_Z) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Fixed addr test with ABC = " << testAddr 
                      << ", expected Z=" << expected_Z << ", got Z=" << dut_Z << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    std::cout << "Starting random test sequence..." << std::endl;
    for (int i = 0; i < 500; i++) {
        // Randomize enable, S, and address bits.
        enable = (std::rand() % 2) ? true : false;
        S = (std::rand() % 2) ? true : false;
        A = (std::rand() % 2) ? true : false;
        B = (std::rand() % 2) ? true : false;
        C = (std::rand() % 2) ? true : false;

        TopModule(enable, S, A, B, C, dut_Z);

        if (enable) {
            golden = ((golden & 0x7F) << 1) | S;
        }
        // Form the address.
        addr[2] = A; addr[1] = B; addr[0] = C;
        expected_Z = (golden >> addr) & 1;
        if (dut_Z != expected_Z) {
            std::cout << "Error at test case " << cycleCount 
                      << ": Random test, enable=" << enable 
                      << ", S=" << S << ", ABC=" << addr 
                      << ", expected Z=" << expected_Z 
                      << ", got Z=" << dut_Z << std::endl;
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
