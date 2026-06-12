#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Prototype
void TopModule(int tsteps,
               int n,
               double u[60][60],
               double v[60][60],
               double p[60][60],
               double q[60][60],
               bool   &valid);

// Reference ADI
void ref_adi(int tsteps,
             int n,
             double u[60][60],
             double v[60][60],
             double p[60][60],
             double q[60][60],
             bool   &valid_ref) {
    double DX = 1.0 / n, DY = 1.0 / n, DT = 1.0 / tsteps;
    double B1 = 2.0, B2 = 1.0;
    double mul1 = B1 * DT / (DX*DX), mul2 = B2 * DT / (DY*DY);
    double a = -mul1*0.5, b = 1.0+mul1, c = a;
    double d = -mul2*0.5, e = 1.0+mul2, f = d;

    for (int t = 1; t <= tsteps; t++) {
        // Column sweep
        for (int i = 1; i < n-1; i++) {
            v[0][i] = 1.0;
            p[i][0] = 0.0;
            q[i][0] = v[0][i];
            for (int j = 1; j < n-1; j++) {
                double denom = a*p[i][j-1] + b;
                p[i][j] = -c/denom;
                q[i][j] = (-d*u[j-1][i] + (1+2*d)*u[j][i] - f*u[j+1][i] - a*q[i][j-1]) / denom;
            }
            v[n-1][i] = 1.0;
            for (int jj = n-2; jj >= 1; jj--) {
                v[jj][i] = p[i][jj]*v[jj+1][i] + q[i][jj];
            }
        }
        // Row sweep
        for (int i = 1; i < n-1; i++) {
            u[i][0] = 1.0;
            p[i][0] = 0.0;
            q[i][0] = u[i][0];
            for (int j = 1; j < n-1; j++) {
                double denom = d*p[i][j-1] + e;
                p[i][j] = -f/denom;
                q[i][j] = (-a*v[i][j-1] + (1+2*a)*v[i][j] - c*v[i][j+1] - d*q[i][j-1]) / denom;
            }
            u[i][n-1] = 1.0;
            for (int jj = n-2; jj >= 1; jj--) {
                u[i][jj] = p[i][jj]*u[i][jj+1] + q[i][jj];
            }
        }
    }
    valid_ref = true;
}

int main() {
    std::srand((unsigned)std::time(nullptr));
    int cycleCount = 0, errorCount = 0;

    const int n = 10, tsteps = 5;
    static double u_dut[60][60], v_dut[60][60], p_dut[60][60], q_dut[60][60];
    static double u_ref[60][60], v_ref[60][60], p_ref[60][60], q_ref[60][60];
    bool valid_dut, valid_ref;
    const double eps = 1e-6;

    // --- 100 Randomized Test Cases ---
    for (int tc = 0; tc < 100; tc++) {
        // Initialize boundaries to 1.0, interior to random [0,1)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double val = (i==0||j==0||i==n-1||j==n-1)
                             ? 1.0
                             : (std::rand() % 1000) / 1000.0;
                u_dut[i][j] = u_ref[i][j] = val;
                v_dut[i][j] = v_ref[i][j] = val;
                p_dut[i][j] = p_ref[i][j] = 0.0;
                q_dut[i][j] = q_ref[i][j] = 0.0;
            }
        }

        // Run DUT and reference
        TopModule(tsteps, n, u_dut, v_dut, p_dut, q_dut, valid_dut);
        ref_adi  (tsteps, n, u_ref, v_ref, p_ref, q_ref, valid_ref);

        // Compare results
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (std::fabs(u_dut[i][j] - u_ref[i][j]) > eps ||
                    std::fabs(v_dut[i][j] - v_ref[i][j]) > eps) {
                    std::cout << "Mismatch at cycle " << cycleCount
                              << " at (" << i << "," << j << "): "
                              << "u_dut=" << u_dut[i][j]
                              << ", u_ref=" << u_ref[i][j]
                              << ", v_dut=" << v_dut[i][j]
                              << ", v_ref=" << v_ref[i][j]
                              << std::endl;
                    errorCount++;
                }
            }
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected valid = " << valid_ref 
                      << ", got " << valid_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
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
