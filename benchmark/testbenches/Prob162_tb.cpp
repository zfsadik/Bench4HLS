#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>

void TopModule(int diagSize, double *matrixA);

int main() {
    const int diagSize = 3;
    // Example SPD matrix A (row-major): 
    double input[diagSize*diagSize] = {
         25, 15, -5,
         15, 18,  0,
         -5,  0, 11
    };

    double expected[diagSize*diagSize];
    double dut     [diagSize*diagSize];
    int errorCount = 0;
    int compareCount = 0;

    // 1) Compute golden L in-place on `expected[]` using same algorithm
    std::memcpy(expected, input, sizeof(expected));
    {
        double *m = expected;
        double tmp = std::sqrt(m[0]);
        m[0] = tmp;
        for (int i = 1; i < diagSize; i++) {
            m[i*diagSize] /= tmp;
        }
        for (int j = 1; j < diagSize; j++) {
            double sum = 0;
            for (int k = 0; k < j; k++) {
                double v = m[j*diagSize + k];
                sum += v * v;
            }
            double diag = std::sqrt(m[j*diagSize + j] - sum);
            m[j*diagSize + j] = diag;
            for (int i = j+1; i < diagSize; i++) {
                double sum2 = 0;
                for (int k = 0; k < j; k++) {
                    sum2 += m[i*diagSize + k] * m[j*diagSize + k];
                }
                m[i*diagSize + j] = (m[i*diagSize + j] - sum2) / diag;
            }
        }
    }

    // 2) Run DUT
    std::memcpy(dut, input, sizeof(dut));
    TopModule(diagSize, dut);

    // 3) Compare each element
    for (int i = 0; i < diagSize; i++) {
        for (int j = 0; j < diagSize; j++) {
            double expected_val = expected[i*diagSize + j];
            double dut_val      = dut    [i*diagSize + j];
            if (std::fabs(dut_val - expected_val) > 1e-6) {
                std::cout << "Error at test case " << compareCount
                          << ": inputs (diagSize, row, col) = ("
                          << diagSize << ", " << i << ", " << j << ")"
                          << " expected = " << expected_val
                          << ", got = "      << dut_val << std::endl;
                errorCount++;
            }
            compareCount++;
        }
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << compareCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << compareCount << " cases." << std::endl;
    }

    return 0;
}
