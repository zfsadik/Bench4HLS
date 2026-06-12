// fft_tb.cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define SIZE 8
typedef float DTYPE;

void TopModule(DTYPE XR[SIZE], DTYPE XI[SIZE]);

// Naïve DFT reference
void ref_compute(const DTYPE XR_in[SIZE], const DTYPE XI_in[SIZE],
                 DTYPE XR_ref[SIZE],    DTYPE XI_ref[SIZE]) {
    const DTYPE TWO_PI = 6.283185307179586f;
    for (int k = 0; k < SIZE; k++) {
        DTYPE sumR = 0, sumI = 0;
        for (int n = 0; n < SIZE; n++) {
            DTYPE ang = TWO_PI * k * n / SIZE;
            DTYPE c = std::cos(ang);
            DTYPE s = std::sin(ang);
            sumR += XR_in[n] * c + XI_in[n] * s;
            sumI += XI_in[n] * c - XR_in[n] * s;
        }
        XR_ref[k] = sumR;
        XI_ref[k] = sumI;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0, errorCount = 0;
    DTYPE XR_in[SIZE], XI_in[SIZE];
    DTYPE XR_dut[SIZE], XI_dut[SIZE];
    DTYPE XR_ref[SIZE], XI_ref[SIZE];

    const int numRandom = 100;
    for (int t = 0; t < numRandom; t++) {
        // random inputs
        for (int i = 0; i < SIZE; i++) {
            XR_in[i] = (std::rand() % 1000) / 100.0f;
            XI_in[i] = (std::rand() % 1000) / 100.0f;
            XR_dut[i] = XR_in[i];
            XI_dut[i] = XI_in[i];
        }

        TopModule(XR_dut, XI_dut);
        ref_compute(XR_in, XI_in, XR_ref, XI_ref);

        // compare each output element
        for (int k = 0; k < SIZE; k++) {
            if (std::fabs(XR_dut[k] - XR_ref[k]) > 1e-3f) {
                std::cout << "Random test error at cycle " << cycleCount
                          << ": index=" << k
                          << ", expected XR=" << XR_ref[k]
                          << ", got XR=" << XR_dut[k] << std::endl;
                errorCount++;
            }
            if (std::fabs(XI_dut[k] - XI_ref[k]) > 1e-3f) {
                std::cout << "Random test error at cycle " << cycleCount
                          << ": index=" << k
                          << ", expected XI=" << XI_ref[k]
                          << ", got XI=" << XI_dut[k] << std::endl;
                errorCount++;
            }
        }
        cycleCount++;
    }

    // Report test summary.
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
