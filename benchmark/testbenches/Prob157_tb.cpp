#include <iostream>
#include <cmath>
#include <cstdlib>

// Prototype of the DUT
void TopModule(const float input[8][8], float output[8][8]);

int main() {
    int cycleCount = 0;
    int errorCount = 0;
    const double PI = 3.14159265358979323846;
    std::srand(0);

    // Run a handful of random blocks
    for (int tc = 0; tc < 5; ++tc) {
        float input[8][8];
        float output[8][8];
        float expected[8][8];

        // Generate random input coefficients in [-256,255]
        for (int u = 0; u < 8; ++u)
            for (int v = 0; v < 8; ++v)
                input[u][v] = static_cast<float>((std::rand() % 512) - 256);

        // Compute DUT result
        TopModule(input, output);

        // Compute reference IDCT
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                double sum = 0.0;
                for (int u = 0; u < 8; ++u) {
                    for (int v = 0; v < 8; ++v) {
                        double cu = (u == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                        double cv = (v == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                        sum += cu * cv
                             * input[u][v]
                             * std::cos((2 * x + 1) * u * PI / 16.0)
                             * std::cos((2 * y + 1) * v * PI / 16.0);
                    }
                }
                expected[x][y] = static_cast<float>(sum * 0.25);
            }
        }

        // Compare each output sample
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                float dut_val = output[x][y];
                float ref_val = expected[x][y];
                if (std::fabs(dut_val - ref_val) > 1e-3f) {
                    std::cout << "Error at test case " << cycleCount
                              << ": pixel (" << x << ", " << y << ")"
                              << " expected = " << ref_val
                              << ", got = " << dut_val << std::endl;
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
