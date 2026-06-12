#include <iostream>
#include <cstdlib>
#include <cmath>

// DUT prototype
void TopModule(const float in[4][4], float out[4][4]);

// Reference inversion in double precision
void referenceInverse(const double in[4][4], double inv[4][4]) {
    double aug[4][8];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            aug[i][j]     = in[i][j];
            aug[i][j + 4] = (i == j) ? 1.0 : 0.0;
        }
    }
    for (int i = 0; i < 4; ++i) {
        int pivot = i;
        double maxv = std::fabs(aug[i][i]);
        for (int r = i + 1; r < 4; ++r) {
            double v = std::fabs(aug[r][i]);
            if (v > maxv) {
                maxv = v; pivot = r;
            }
        }
        if (pivot != i) {
            for (int c = 0; c < 8; ++c) {
                double tmp = aug[i][c];
                aug[i][c] = aug[pivot][c];
                aug[pivot][c] = tmp;
            }
        }
        double diag = aug[i][i];
        for (int c = 0; c < 8; ++c) {
            aug[i][c] /= diag;
        }
        for (int r = 0; r < 4; ++r) {
            if (r == i) continue;
            double factor = aug[r][i];
            for (int c = 0; c < 8; ++c) {
                aug[r][c] -= factor * aug[i][c];
            }
        }
    }
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            inv[i][j] = aug[i][j + 4];
}

int main() {
    std::srand(0);
    int cycleCount = 0;
    int errorCount = 0;
    const float eps = 1e-3f;

    // Test a few random invertible matrices
    for (int tc = 0; tc < 10; ++tc) {
        float in[4][4], out[4][4];
        double din[4][4], expected[4][4];

        // generate random matrix with entries in [-10,10]
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                float v = (std::rand() % 2001 - 1000) / 100.0f;
                in[i][j] = v;
                din[i][j] = v;
            }

        // compute reference inverse
        referenceInverse(din, expected);

        // run DUT
        TopModule(in, out);

        // compare element‑wise
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float dut_v = out[i][j];
                float ref_v = static_cast<float>(expected[i][j]);
                if (std::isnan(dut_v) || std::isinf(dut_v)) {
                    std::cout << "Error at test case " << tc
                              << ": NaN/Inf output at row=" << i
                              << " col=" << j << std::endl;
                    errorCount++;
                } else if (std::fabs(dut_v - ref_v) > eps) {
                    std::cout << "Error at test case " << tc
                              << ": inputs (matrix, row, col) = ("
                              << tc << ", " << i << ", " << j << ")"
                              << " expected = " << ref_v
                              << ", got = " << dut_v << std::endl;
                    errorCount++;
                }
                cycleCount++;
            }
        }
    }

    // Report test summary
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
