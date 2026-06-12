#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_fixed.h"

void TopModule(ap_fixed<8,1>  sample_in, const ap_fixed<8,1> coeffs[4], ap_fixed<19,4> &sample_out);

// Reference model
void ref_compute(ap_fixed<8,1>  sample_in, const ap_fixed<8,1> coeffs[4],
                 ap_fixed<19,4> &sample_out_ref, ap_fixed<19,4> regs_ref[4])
{
    ap_fixed<19,4> tmp = 0;
    for (int i = 3; i >= 0; --i) {
        tmp           = (i == 0) ? ap_fixed<19,4>(0) : regs_ref[i-1];
        regs_ref[i]   = sample_in * coeffs[3-i] + tmp;
    }
    sample_out_ref = regs_ref[3];
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Constant coefficient set used for the whole run
    const ap_fixed<8,1> coeffs[4] = { 0.30011, 0.90032, 0.90032, 0.30011 };

    ap_fixed<8,1>  x;
    ap_fixed<19,4> y_dut, y_ref;
    ap_fixed<19,4> regs_ref[4] = {0, 0, 0, 0};

    int cycleCount = 0;
    int errorCount = 0;

    // -------- Phase 1 : deterministic stimuli (impulse etc.) ------------
    const ap_fixed<8,1> fixedVec[] = {
        0.0, 1.0, 0.0, 0.0, 0.0,    // impulse response should equal coeffs
        -1.0, 0.5, -0.5, 0.25       // mix of signs
    };

    for (ap_fixed<8,1> v : fixedVec) {
        x = v;
        TopModule(x, coeffs, y_dut);
        ref_compute(x, coeffs, y_ref, regs_ref);

        if (y_dut != y_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << " : expected " << y_ref
                      << ", got " << y_dut << '\n';
            ++errorCount;
        }
        ++cycleCount;
    }

    // -------- Phase 2 : 1000 random samples ----------------------------
    const int numRandom = 1000;
    for (int i = 0; i < numRandom; ++i) {
        // random in ­‑0.9 … +0.9
        double rnd = (static_cast<double>(std::rand()) / RAND_MAX) * 1.8 - 0.9;
        x = rnd;

        TopModule(x, coeffs, y_dut);
        ref_compute(x, coeffs, y_ref, regs_ref);

        if (y_dut != y_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << " : expected " << y_ref
                      << ", got " << y_dut << '\n';
            ++errorCount;
        }
        ++cycleCount;
    }

    // -------- Summary ---------------------------------------------------
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
