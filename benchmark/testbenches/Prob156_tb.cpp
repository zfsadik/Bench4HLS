#include <iostream>
#include <cstdlib>

constexpr int IMG_H = 8;
constexpr int IMG_W = 8;
constexpr int K     = 5;

void TopModule(const int input[IMG_H][IMG_W], int output[IMG_H][IMG_W]);

static void compute_expected(const int input[IMG_H][IMG_W], int expected[IMG_H][IMG_W]) {
    static const int kernel[K][K] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };
    for (int i = 0; i < IMG_H; i++) {
        for (int j = 0; j < IMG_W; j++) {
            int sum = 0;
            for (int ki = 0; ki < K; ki++) {
                for (int kj = 0; kj < K; kj++) {
                    int ii = i + ki - (K/2);
                    int jj = j + kj - (K/2);
                    if (ii >= 0 && ii < IMG_H && jj >= 0 && jj < IMG_W) {
                        sum += input[ii][jj] * kernel[ki][kj];
                    }
                }
            }
            expected[i][j] = sum;
        }
    }
}

static int run_test(const int input[IMG_H][IMG_W], int &cycleCount) {
    int expected[IMG_H][IMG_W];
    int dut[IMG_H][IMG_W];

    compute_expected(input, expected);
    TopModule(input, dut);

    int errors = 0;
    for (int i = 0; i < IMG_H; i++) {
        for (int j = 0; j < IMG_W; j++) {
            if (dut[i][j] != expected[i][j]) {
                std::cout << "Error at cycle " << cycleCount
                          << ": (row, col) = (" << i << ", " << j << ")"
                          << " expected " << expected[i][j]
                          << ", got " << dut[i][j] << std::endl;
                errors++;
            }
            cycleCount++;
        }
    }
    return errors;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Corner case: all zeros
    {
        int input[IMG_H][IMG_W] = {};
        errorCount += run_test(input, cycleCount);
    }

    // Corner case: all max (10)
    {
        int input[IMG_H][IMG_W];
        for (int i = 0; i < IMG_H; i++)
            for (int j = 0; j < IMG_W; j++)
                input[i][j] = 10;
        errorCount += run_test(input, cycleCount);
    }

    // Corner case: single nonzero pixel at center
    {
        int input[IMG_H][IMG_W] = {};
        input[3][3] = 7;
        errorCount += run_test(input, cycleCount);
    }

    // Corner case: single nonzero pixel at corner
    {
        int input[IMG_H][IMG_W] = {};
        input[0][0] = 5;
        errorCount += run_test(input, cycleCount);
    }

    // Random test cases
    for (int t = 0; t < 3; t++) {
        int input[IMG_H][IMG_W];
        for (int i = 0; i < IMG_H; i++)
            for (int j = 0; j < IMG_W; j++)
                input[i][j] = std::rand() % 11;
        errorCount += run_test(input, cycleCount);
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
