#include <iostream>
#include <cmath>

#define MAX_ROW 32
#define MAX_COL 32
typedef float float32_t;

void TopModule(float32_t Qr_i[MAX_ROW*MAX_COL],
               float32_t Qi_i[MAX_ROW*MAX_COL],
               int col, int row,
               float32_t br_i[MAX_ROW],
               float32_t bi_i[MAX_ROW],
               float32_t xr_o[MAX_COL],
               float32_t xi_o[MAX_COL]);

int main() {
    int errorCount = 0, cycleCount = 0;
    const float eps = 1e-5f;

    // Test 1: Q=I_4, b=[1,2,3,4] real — expect x=b (since Q=I, R=I)
    {
        const int col = 4, row = 4;
        float Qr_i[32*32] = {0}, Qi_i[32*32] = {0};
        float br_i[32] = {0}, bi_i[32] = {0}, xr_o[32], xi_o[32];

        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                Qr_i[i*col + j] = (i == j) ? 1.0f : 0.0f;
            }
            br_i[i] = float(i + 1);
        }

        TopModule(Qr_i, Qi_i, col, row, br_i, bi_i, xr_o, xi_o);

        for (int i = 0; i < col; i++) {
            if (std::fabs(xr_o[i] - br_i[i]) > eps) {
                std::cout << "Test1 error at i=" << i
                          << ": xr=" << xr_o[i]
                          << ", expected=" << br_i[i] << "\n";
                errorCount++;
            }
            if (std::fabs(xi_o[i]) > eps) {
                std::cout << "Test1 error at i=" << i
                          << ": xi=" << xi_o[i] << ", expected=0\n";
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Test 2: Q=I_2, complex b=[1+2j, 3+4j] — expect x=[1+2j, 3+4j]
    {
        const int col = 2, row = 2;
        float Qr_i[32*32] = {0}, Qi_i[32*32] = {0};
        float br_i[32] = {0}, bi_i[32] = {0}, xr_o[32], xi_o[32];

        for (int i = 0; i < row; i++) Qr_i[i*col + i] = 1.0f;
        br_i[0] = 1.0f; bi_i[0] = 2.0f;
        br_i[1] = 3.0f; bi_i[1] = 4.0f;

        TopModule(Qr_i, Qi_i, col, row, br_i, bi_i, xr_o, xi_o);

        float exp_r[2] = {1.0f, 3.0f};
        float exp_i[2] = {2.0f, 4.0f};
        for (int i = 0; i < col; i++) {
            if (std::fabs(xr_o[i] - exp_r[i]) > eps) {
                std::cout << "Test2 error at i=" << i
                          << ": xr=" << xr_o[i]
                          << ", expected=" << exp_r[i] << "\n";
                errorCount++;
            }
            if (std::fabs(xi_o[i] - exp_i[i]) > eps) {
                std::cout << "Test2 error at i=" << i
                          << ": xi=" << xi_o[i]
                          << ", expected=" << exp_i[i] << "\n";
                errorCount++;
            }
            cycleCount++;
        }
    }

    // Test 3: overdetermined 4×2, Q=[[1,0],[0,1],[0,0],[0,0]], b=[1,2,3,4] — expect x=[1,2]
    {
        const int col = 2, row = 4;
        float Qr_i[32*32] = {0}, Qi_i[32*32] = {0};
        float br_i[32] = {0}, bi_i[32] = {0}, xr_o[32], xi_o[32];

        Qr_i[0*col + 0] = 1.0f;
        Qr_i[1*col + 1] = 1.0f;
        for (int i = 0; i < row; i++) br_i[i] = float(i + 1);

        TopModule(Qr_i, Qi_i, col, row, br_i, bi_i, xr_o, xi_o);

        float exp_r[2] = {1.0f, 2.0f};
        for (int i = 0; i < col; i++) {
            if (std::fabs(xr_o[i] - exp_r[i]) > eps) {
                std::cout << "Test3 error at i=" << i
                          << ": xr=" << xr_o[i]
                          << ", expected=" << exp_r[i] << "\n";
                errorCount++;
            }
            if (std::fabs(xi_o[i]) > eps) {
                std::cout << "Test3 error at i=" << i
                          << ": xi=" << xi_o[i] << ", expected=0\n";
                errorCount++;
            }
            cycleCount++;
        }
    }

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
