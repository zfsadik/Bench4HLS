#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<16> scancode, bool &left, bool &down, bool &right, bool &up);

void ref_compute(ap_uint<16> scancode, bool &left, bool &down, bool &right, bool &up) {
    left  = (scancode == 0xE06B);
    down  = (scancode == 0xE072);
    right = (scancode == 0xE074);
    up    = (scancode == 0xE075);
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));
    
    int cycleCount = 0;
    int errorCount = 0;
    
    ap_uint<16> scancode;
    bool left_dut = false, down_dut = false, right_dut = false, up_dut = false;
    bool left_ref = false, down_ref = false, right_ref = false, up_ref = false;
    
    // --- Phase 1: Fixed Test Cases ---
    const int numFixedTests = 9;
    ap_uint<16> fixedTests[numFixedTests] = { 0x0000, 0x0001, 0xE075, 0xE06B, 0xE06C, 0xE072, 0xE074, 0x1234, 0xFFFF };
    
    for (int i = 0; i < numFixedTests; i++) {
        scancode = fixedTests[i];
        TopModule(scancode, left_dut, down_dut, right_dut, up_dut);
        ref_compute(scancode, left_ref, down_ref, right_ref, up_ref);
        
        // Compare each output using the prescribed reporting format.
        if (left_dut != left_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected left = " << left_ref 
                      << ", got " << left_dut << std::endl;
            errorCount++;
        }
        if (down_dut != down_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected down = " << down_ref 
                      << ", got " << down_dut << std::endl;
            errorCount++;
        }
        if (right_dut != right_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected right = " << right_ref 
                      << ", got " << right_dut << std::endl;
            errorCount++;
        }
        if (up_dut != up_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected up = " << up_ref 
                      << ", got " << up_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Phase 2: Random Test Cases ---
    const int numRandomTests = 30000;
    for (int i = 0; i < numRandomTests; i++) {
        // Generate a random 16-bit scancode.
        scancode = std::rand() & 0xFFFF;
        TopModule(scancode, left_dut, down_dut, right_dut, up_dut);
        ref_compute(scancode, left_ref, down_ref, right_ref, up_ref);
        
        if (left_dut != left_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected left = " << left_ref 
                      << ", got " << left_dut << std::endl;
            errorCount++;
        }
        if (down_dut != down_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected down = " << down_ref 
                      << ", got " << down_dut << std::endl;
            errorCount++;
        }
        if (right_dut != right_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected right = " << right_ref 
                      << ", got " << right_dut << std::endl;
            errorCount++;
        }
        if (up_dut != up_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected up = " << up_ref 
                      << ", got " << up_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Print final test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount 
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount 
                  << " cases." << std::endl;
    }
    
    return 0;
}
