#include <iostream>
#include <cstdlib>
#include <ap_int.h>

void TopModule(ap_uint<3> y, ap_uint<1> w, ap_uint<1> &Y2);

ap_uint<1> ref_comb(ap_uint<3> y, ap_uint<1> w) {
    ap_uint<4> idx = (ap_uint<4>(y) << 1) | (ap_uint<1>)w;
    switch (idx) {
        case 0x0: return 0;
        case 0x1: return 0;
        case 0x2: return 0;
        case 0x3: return 0;
        case 0x4: return 1;
        case 0x5: return 0;
        case 0x6: return 1;
        case 0x7: return 0;
        case 0x8: return 1;
        case 0x9: return 0;
        case 0xA: return 0;
        case 0xB: return 0;
        default:  return 0;
    }
}

int main() {
    const int N = 100;           // number of clock edges to test
    ap_uint<3> y;
    ap_uint<1> w;
    ap_uint<1> Y2_dut, Y2_ref;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    for (int i = 0; i < N; i++) {
        y = std::rand() & 0x7;   // random 3‑bit state
        w = std::rand() & 1;

        // Call DUT and reference
        TopModule(y, w, Y2_dut);
        Y2_ref = ref_comb(y, w);

        // Compare and report
        if (Y2_dut != Y2_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << Y2_ref
                      << ", got " << Y2_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Final summary
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
