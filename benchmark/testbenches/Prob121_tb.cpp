#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(bool a, bool b, bool c, bool d, bool & out);

// Reference model function.
bool ref_model(bool a, bool b, bool c, bool d) {
    ap_uint<4> in_val = ((a ? 1 : 0) << 3) |
                        ((b ? 1 : 0) << 2) |
                        ((c ? 1 : 0) << 1) |
                         (d ? 1 : 0);
    bool result;
    switch(in_val) {
        case 0x0: result = 0; break;  // 0000
        case 0x1: result = 0; break;  // 0001
        case 0x3: result = 1; break;  // 0011
        case 0x2: result = 1; break;  // 0010
        case 0x4: result = 0; break;  // 0100: don't-care, chosen 0
        case 0x5: result = 0; break;  // 0101
        case 0x7: result = 0; break;  // 0111
        case 0x6: result = 0; break;  // 0110
        case 0xC: result = 1; break;  // 1100
        case 0xD: result = 0; break;  // 1101: don't-care, chosen 0
        case 0xF: result = 1; break;  // 1111
        case 0xE: result = 1; break;  // 1110
        case 0x8: result = 1; break;  // 1000
        case 0x9: result = 0; break;  // 1001: don't-care, chosen 0
        case 0xB: result = 1; break;  // 1011
        case 0xA: result = 1; break;  // 1010
        default:  result = 0; break;
    }
    return result;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool a, b, c, d;
    bool out_dut, out_ref;

    // --- Phase 1: Sequential Test on 16 Combinations ---
    for (int count = 0; count < 16; count++) {
        a = ((count >> 3) & 1) ? true : false;
        b = ((count >> 2) & 1) ? true : false;
        c = ((count >> 1) & 1) ? true : false;
        d = ((count >> 0) & 1) ? true : false;

        TopModule(a, b, c, d, out_dut);
        out_ref = ref_model(a, b, c, d);

        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << out_ref
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Testing for 200 Cases ---
    for (int i = 0; i < 200; i++) {
        // Generate a random 4-bit number.
        int rnd = rand() & 0xF;
        a = ((rnd >> 3) & 1) ? true : false;
        b = ((rnd >> 2) & 1) ? true : false;
        c = ((rnd >> 1) & 1) ? true : false;
        d = ((rnd >> 0) & 1) ? true : false;

        TopModule(a, b, c, d, out_dut);
        out_ref = ref_model(a, b, c, d);

        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << out_ref
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Test Summary ---
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
