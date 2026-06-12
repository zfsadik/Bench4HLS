#include <iostream>
#include <complex>
#include <cmath>
#include <cstdlib>

void TopModule(std::complex<float> x[64]);

int main() {
    const int NUM_TESTS = 5;
    const float TOL     = 1e-3f;
    int errorCount = 0;
    int cycleCount = 0;

    for (int t = 0; t < NUM_TESTS; t++) {
        std::complex<float> input[64], expected[64];

        for (int n = 0; n < 64; n++) {
            float r = static_cast<float>(std::rand()) / RAND_MAX;
            float im = static_cast<float>(std::rand()) / RAND_MAX;
            input[n]    = {r, im};
            expected[n] = {0.0f, 0.0f};
        }

        // Compute golden DFT
        for (int k = 0; k < 64; k++) {
            for (int n = 0; n < 64; n++) {
                float ang = -2.0f * M_PI * k * n / 64.0f;
                float c   = std::cos(ang);
                float s   = std::sin(ang);
                expected[k].real(expected[k].real() + input[n].real()*c - input[n].imag()*s);
                expected[k].imag(expected[k].imag() + input[n].real()*s + input[n].imag()*c);
            }
        }

        // Run DUT
        std::complex<float> dut[64];
        for (int i = 0; i < 64; i++) dut[i] = input[i];
        TopModule(dut);

        // Compare
        for (int k = 0; k < 64; k++) {
            float dr = std::fabs(dut[k].real() - expected[k].real());
            float di = std::fabs(dut[k].imag() - expected[k].imag());
            if (dr > TOL || di > TOL) {
                std::cout << "Error at test case " << t
                          << ": index k=" << k
                          << " expected (" << expected[k].real() << ", " << expected[k].imag() << ")"
                          << ", got ("    << dut[k].real()      << ", " << dut[k].imag()      << ")"
                          << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
