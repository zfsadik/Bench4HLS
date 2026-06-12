#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// DUT prototype
void TopModule(int    tmax,
               int    nx,
               int    ny,
               double ex[200][240],
               double ey[200][240],
               double hz[200][240],
               double _fict_[100],
               bool   &valid);

// Reference FDTD
void ref_fdtd2d(int    tmax,
                int    nx,
                int    ny,
                double ex[200][240],
                double ey[200][240],
                double hz[200][240],
                double _fict_[100],
                bool   &valid_ref) {
    for (int t = 0; t < tmax; t++) {
        for (int j = 0; j < ny; j++) {
            ey[0][j] = _fict_[t];
        }
        for (int i = 1; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                ey[i][j] -= 0.5 * (hz[i][j] - hz[i-1][j]);
            }
        }
        for (int i = 0; i < nx; i++) {
            for (int j = 1; j < ny; j++) {
                ex[i][j] -= 0.5 * (hz[i][j] - hz[i][j-1]);
            }
        }
        for (int i = 0; i < nx-1; i++) {
            for (int j = 0; j < ny-1; j++) {
                hz[i][j] -= 0.7 * (
                    ex[i][j+1] - ex[i][j]
                  + ey[i+1][j] - ey[i][j]
                );
            }
        }
    }
    valid_ref = true;
}

int main() {
    std::srand((unsigned)std::time(nullptr));
    const int tmax = 10, nx = 50, ny = 60;
    static double ex_dut[200][240], ey_dut[200][240], hz_dut[200][240], fict_dut[100];
    static double ex_ref[200][240], ey_ref[200][240], hz_ref[200][240], fict_ref[100];
    bool valid_dut, valid_ref;
    int cycleCount = 0, errorCount = 0;
    const double eps = 1e-9;

    // --- Phase 1: Fixed Pattern ---
    // Initialize everything to zero
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            ex_dut[i][j] = ey_dut[i][j] = hz_dut[i][j] =
            ex_ref[i][j] = ey_ref[i][j] = hz_ref[i][j] = 0.0;
        }
    }
    // Fictitious boundary: a ramp
    for (int t = 0; t < tmax; t++) {
        fict_dut[t] = fict_ref[t] = double(t);
    }

    TopModule(tmax,nx,ny,ex_dut,ey_dut,hz_dut,fict_dut,valid_dut);
    ref_fdtd2d(tmax,nx,ny,ex_ref,ey_ref,hz_ref,fict_ref,valid_ref);

    // Compare fixed results
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            if (std::fabs(ex_dut[i][j] - ex_ref[i][j]) > eps ||
                std::fabs(ey_dut[i][j] - ey_ref[i][j]) > eps ||
                std::fabs(hz_dut[i][j] - hz_ref[i][j]) > eps) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << " at (" << i << "," << j << ")\n";
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

    // --- Phase 2: 100 Random Tests ---
    for (int tc = 0; tc < 100; tc++) {
        // Random init
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                double v = (std::rand()%1000)/1000.0;
                ex_dut[i][j] = ex_ref[i][j] = v;
                ey_dut[i][j] = ey_ref[i][j] = v;
                hz_dut[i][j] = hz_ref[i][j] = v;
            }
        }
        for (int t = 0; t < tmax; t++) {
            double v = (std::rand()%1000)/1000.0;
            fict_dut[t] = fict_ref[t] = v;
        }

        TopModule(tmax,nx,ny,ex_dut,ey_dut,hz_dut,fict_dut,valid_dut);
        ref_fdtd2d(tmax,nx,ny,ex_ref,ey_ref,hz_ref,fict_ref,valid_ref);

        // Compare
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                if (std::fabs(ex_dut[i][j] - ex_ref[i][j]) > eps ||
                    std::fabs(ey_dut[i][j] - ey_ref[i][j]) > eps ||
                    std::fabs(hz_dut[i][j] - hz_ref[i][j]) > eps) {
                    std::cout << "Mismatch at cycle " << cycleCount
                              << " at (" << i << "," << j << ")\n";
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

    // --- Phase 3: Stress Test at Maximum Dimensions ---
    {
        const int tmax_max = 100, nx_max = 200, ny_max = 240;
        for (int i = 0; i < nx_max; i++) {
            for (int j = 0; j < ny_max; j++) {
                double v = (std::rand()%1000)/1000.0;
                ex_dut[i][j] = ex_ref[i][j] = v;
                ey_dut[i][j] = ey_ref[i][j] = v;
                hz_dut[i][j] = hz_ref[i][j] = v;
            }
        }
        for (int t = 0; t < tmax_max; t++) {
            double v = (std::rand()%1000)/1000.0;
            fict_dut[t] = fict_ref[t] = v;
        }

        TopModule(tmax_max,nx_max,ny_max,ex_dut,ey_dut,hz_dut,fict_dut,valid_dut);
        ref_fdtd2d(tmax_max,nx_max,ny_max,ex_ref,ey_ref,hz_ref,fict_ref,valid_ref);

        for (int i = 0; i < nx_max; i++) {
            for (int j = 0; j < ny_max; j++) {
                if (std::fabs(ex_dut[i][j] - ex_ref[i][j]) > eps ||
                    std::fabs(ey_dut[i][j] - ey_ref[i][j]) > eps ||
                    std::fabs(hz_dut[i][j] - hz_ref[i][j]) > eps) {
                    std::cout << "Mismatch at cycle " << cycleCount
                              << " at (" << i << "," << j << ")\n";
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
