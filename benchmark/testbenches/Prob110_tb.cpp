#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<8> code, ap_uint<4> &out, ap_uint<1> &valid);

void ref_compute(ap_uint<8> code, ap_uint<4> &out_ref, ap_uint<1> &valid_ref) {
    out_ref = 0;
    valid_ref = 0;
    switch(code) {
        case 0x45: out_ref = 0; valid_ref = 1; break;
        case 0x16: out_ref = 1; valid_ref = 1; break;
        case 0x1e: out_ref = 2; valid_ref = 1; break;
        case 0x26: out_ref = 3; valid_ref = 1; break;
        case 0x25: out_ref = 4; valid_ref = 1; break;
        case 0x2e: out_ref = 5; valid_ref = 1; break;
        case 0x36: out_ref = 6; valid_ref = 1; break;
        case 0x3d: out_ref = 7; valid_ref = 1; break;
        case 0x3e: out_ref = 8; valid_ref = 1; break;
        case 0x46: out_ref = 9; valid_ref = 1; break;
    }
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    ap_uint<8> code;
    ap_uint<4> out_dut;
    ap_uint<4> out_ref;
    ap_uint<1> valid_dut;
    ap_uint<1> valid_ref;

    // --- Phase 0: Hardcoded Negative Test Cases ---
    const int numNeg = 5;
    ap_uint<8> negCodes[numNeg] = { 0x00, 0x01, 0xFF, 0x20, 0x5A };
    for (int i = 0; i < numNeg; i++) {
        code = negCodes[i];
        TopModule(code, out_dut, valid_dut);
        if (out_dut != 0) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": invalid code 0x" << std::hex << (int)code.to_uint()
                      << " expected out=0, got " << std::dec << (int)out_dut.to_uint() << std::endl;
            errorCount++;
        }
        if (valid_dut != 0) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": invalid code 0x" << std::hex << (int)code.to_uint()
                      << " expected valid=0, got " << std::dec << (int)valid_dut.to_uint() << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 1: Fixed Test Cases ---
    const int numFixed = 10;
    ap_uint<8> fixedCodes[numFixed] = { 0x45, 0x16, 0x1e, 0x26, 0x25,
                                         0x2e, 0x36, 0x3d, 0x3e, 0x46 };
    for (int i = 0; i < numFixed; i++) {
        code = fixedCodes[i];
        TopModule(code, out_dut, valid_dut);
        ref_compute(code, out_ref, valid_ref);

        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out = " << out_ref
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected valid = " << valid_ref
                      << ", got " << valid_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Cases ---
    const int numRandom = 1000;
    for (int i = 0; i < numRandom; i++) {
        code = std::rand() & 0xFF;  // Random 8-bit value.
        TopModule(code, out_dut, valid_dut);
        ref_compute(code, out_ref, valid_ref);

        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out = " << out_ref
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected valid = " << valid_ref
                      << ", got " << valid_dut << std::endl;
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
