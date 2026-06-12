#include <iostream>
#include <vector>
#include <cstdlib>

void TopModule(int* in1, int* in2, int* out_r, int size, int rep_count);

// Reference matrix‑multiply for verification
void ref_matmul(int* C, int* A, int* B, int M) {
    // initialize
    for (int i = 0; i < M*M; ++i) C[i] = 0;
    // C = A × B
    for (int row = 0; row < M; ++row) {
        for (int col = 0; col < M; ++col) {
            for (int k = 0; k < M; ++k) {
                C[row*M + col] += A[row*M + k] * B[k*M + col];
            }
        }
    }
}

int main() {
    constexpr int size      = 8;
    constexpr int rep_count = 8;

    std::vector<int> A(size*size), B(size*size), C(size*size), gold(size*size);

    // Random data generator
    srand(42);
    for (int i = 0; i < size*size; ++i) {
        A[i] = rand() % 11;
        B[i] = rand() % 11;
    }

    // Compute golden result
    ref_matmul(gold.data(), A.data(), B.data(), size);

    // Run HLS design
    TopModule(A.data(), B.data(), C.data(), size, rep_count);

    // Compare
    int errorCount = 0;
    int cycleCount = 0;
    for (int idx = 0; idx < size*size; ++idx) {
        int expected = gold[idx];
        int dut      = C[idx];
        if (dut != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (size, rep_count) = ("
                      << size << ", " << rep_count << ")"
                      << " expected out = " << expected
                      << ", got out = " << dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases."
                  << std::endl;
    }

    return 0;
}
