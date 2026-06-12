#include <iostream>
#include <cmath>

// DUT declaration
extern void TopModule(double float_n,
                      double data[100][80],
                      double corr[80][80],
                      double mean[80],
                      double stddev[80]);

int main() {
    const double float_n = 100.0;
    static double data[100][80];
    static double orig[100][80];
    static double corr[80][80];
    static double mean[80], stddev[80];
    static double exp_mean[80], exp_stddev[80], exp_corr[80][80];

    // --- Initialize data ---
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 80; ++j) {
            orig[i][j] = data[i][j] = 1.0 + (double)((i * j) % 10);
        }
    }

    // --- Reference computation ---

    // Mean
    for (int j = 0; j < 80; ++j) {
        double s = 0.0;
        for (int i = 0; i < 100; ++i) s += orig[i][j];
        exp_mean[j] = s / float_n;
    }

    // Stddev
    const double eps = 0.1;
    for (int j = 0; j < 80; ++j) {
        double ss = 0.0;
        for (int i = 0; i < 100; ++i) {
            double d = orig[i][j] - exp_mean[j];
            ss += d * d;
        }
        double var = ss / float_n;
        double s   = std::sqrt(var);
        exp_stddev[j] = (s <= eps ? 1.0 : s);
    }

    // Prepare for correlation
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 80; ++j) {
            orig[i][j] = (orig[i][j] - exp_mean[j]) / (std::sqrt(float_n) * exp_stddev[j]);
        }
    }

    // Correlation matrix
    for (int i = 0; i < 80; ++i)
      for (int j = 0; j < 80; ++j)
        exp_corr[i][j] = 0.0;

    for (int i = 0; i < 80; ++i) {
        exp_corr[i][i] = 1.0;
        for (int j = i + 1; j < 80; ++j) {
            double s = 0.0;
            for (int k = 0; k < 100; ++k) {
                s += orig[k][i] * orig[k][j];
            }
            exp_corr[i][j] = s;
            exp_corr[j][i] = s;
        }
    }

    // --- Invoke DUT ---
    TopModule(float_n, data, corr, mean, stddev);

    // --- Self-checking ---
    int errorCount = 0;
    int cycleCount = 0;

    // Check means
    for (int j = 0; j < 80; ++j) {
        if (std::fabs(mean[j] - exp_mean[j]) > 1e-9) {
            std::cout << "Error at test case " << cycleCount
                      << ": j=" << j
                      << " expected mean=" << exp_mean[j]
                      << ", got mean=" << mean[j] << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Check stddevs
    for (int j = 0; j < 80; ++j) {
        if (std::fabs(stddev[j] - exp_stddev[j]) > 1e-9) {
            std::cout << "Error at test case " << cycleCount
                      << ": j=" << j
                      << " expected stddev=" << exp_stddev[j]
                      << ", got stddev=" << stddev[j] << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Check correlation matrix
    for (int i = 0; i < 80; ++i) {
        for (int j = 0; j < 80; ++j) {
            if (std::fabs(corr[i][j] - exp_corr[i][j]) > 1e-9) {
                std::cout << "Error at test case " << cycleCount
                          << ": i=" << i << ", j=" << j
                          << " expected corr=" << exp_corr[i][j]
                          << ", got corr=" << corr[i][j] << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
