#include <iostream>
#include <cstdlib>
#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, ap_uint<1>& q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    ap_uint<1> a, b, c, d;
    ap_uint<1> q_dut;
    ap_uint<1> q_ref;

    // Reference model: truth table from waveform.
    // Index = {a,b,c,d} as 4-bit value.
    const ap_uint<1> truth[16] = {1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1};
    auto ref_model = [&truth](ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d) -> ap_uint<1> {
        int idx = (a.to_uint() << 3) | (b.to_uint() << 2) | (c.to_uint() << 1) | d.to_uint();
        return truth[idx];
    };

    // Systematic test: enumerate all 16 combinations of 4 inputs.
    for (int i = 0; i < 16; i++) {
        a = (i >> 3) & 0x1;
        b = (i >> 2) & 0x1;
        c = (i >> 1) & 0x1;
        d = i & 0x1;

        TopModule(a, b, c, d, q_dut);
        q_ref = ref_model(a, b, c, d);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Additional random tests (e.g., 100 random cases).
    for (int i = 0; i < 100; i++) {
        a = rand() % 2;
        b = rand() % 2;
        c = rand() % 2;
        d = rand() % 2;

        TopModule(a, b, c, d, q_dut);
        q_ref = ref_model(a, b, c, d);

        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected " << q_ref
                      << ", got " << q_dut << std::endl;
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
