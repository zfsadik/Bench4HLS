#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_fixed.h"

// DUT prototype
void TopModule(ap_fixed<8,1> sample_in, ap_fixed<19,4> &sample_out);

// Reference model
void ref_compute(ap_fixed<8,1> sample_in, ap_fixed<19,4> &sample_out_ref,
                 ap_fixed<8,1> regs_ref[4]) {

    const ap_fixed<16,1> h[2] = { 0.30011, 0.90032 };

    // shift and insert
    regs_ref[3] = regs_ref[2];
    regs_ref[2] = regs_ref[1];
    regs_ref[1] = regs_ref[0];
    regs_ref[0] = sample_in;

    ap_fixed<9,2> sym0 = regs_ref[0] + regs_ref[3];
    ap_fixed<9,2> sym1 = regs_ref[1] + regs_ref[2];
    sample_out_ref = h[0] * sym0 + h[1] * sym1;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ap_fixed<8,1>  x;
    ap_fixed<19,4> y_dut, y_ref;
    ap_fixed<8,1>  regs_ref[4] = { 0, 0, 0, 0 };

    int cycleCount = 0;
    int errorCount = 0;

    // ---------- Phase 1 : impulse & corner cases ----------
    const ap_fixed<8,1> fixedVec[] = {
        0.0,            // warm‑up zero
        1.0, 0.0, 0.0, 0.0,   // impulse → should output coeffs
        -1.0, 0.5, -0.5, 0.25 // sign & magnitude mix
    };

    for (ap_fixed<8,1> v : fixedVec) {
        x = v;
        TopModule(x, y_dut);
        ref_compute(x, y_ref, regs_ref);

        if (y_dut != y_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << " : expected " << y_ref
                      << ", got " << y_dut << '\n';
            ++errorCount;
        }
        ++cycleCount;
    }

    // ---------- Phase 2 : 1000 random samples ----------
    const int numRandom = 1000;
    for (int i = 0; i < numRandom; ++i) {
        // Generate random value in [‑0.9 , +0.9)
        double rnd = (static_cast<double>(std::rand()) / RAND_MAX) * 1.8 - 0.9;
        x = rnd;   // automatic cast to ap_fixed<8,1>

        TopModule(x, y_dut);
        ref_compute(x, y_ref, regs_ref);

        if (y_dut != y_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << " : expected " << y_ref
                      << ", got " << y_dut << '\n';
            ++errorCount;
        }
        ++cycleCount;
    }

    // ---------- Summary ----------
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched.\n";
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of "
                  << cycleCount << " cases.\n";
    }

    return 0;
}
