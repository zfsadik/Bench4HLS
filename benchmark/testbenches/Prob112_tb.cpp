#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<1> &f);

// Reference function: computes the expected output using the same mapping.
void ref_compute(ap_uint<4> x, ap_uint<1> &f_ref) {

    switch(x) {
        case 0x0: f_ref = false; break;  // don't-care chosen as 0
        case 0x1: f_ref = false; break;
        case 0x2: f_ref = true;  break;
        case 0x3: f_ref = true;  break;
        case 0x4: f_ref = false; break;
        case 0x5: f_ref = false; break;  // don't-care chosen as 0
        case 0x6: f_ref = true;  break;
        case 0x7: f_ref = true;  break;
        case 0x8: f_ref = false; break;
        case 0x9: f_ref = false; break;
        case 0xA: f_ref = false; break;  // don't-care chosen as 0
        case 0xB: f_ref = false; break;  // don't-care chosen as 0
        case 0xC: f_ref = true;  break;
        case 0xD: f_ref = true;  break;
        case 0xE: f_ref = false; break;
        case 0xF: f_ref = false; break;  // don't-care chosen as 0
        default:  f_ref = false; break;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<4> x;
    ap_uint<1> f_dut = 0;
    ap_uint<1> f_ref  = 0;

    // --- Phase 1: Fixed Test Cases ---
    for (unsigned int val = 0; val < 16; val++) {
        x = val;
        TopModule(x, f_dut);
        ref_compute(x, f_ref);
        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << f_ref
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Cases ---
    const int numRandom = 50;
    for (int i = 0; i < numRandom; i++) {
        x = std::rand() & 0xF; // Random 4-bit value.
        TopModule(x, f_dut);
        ref_compute(x, f_ref);
        if (f_dut != f_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << f_ref
                      << ", got " << f_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

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
