#include <cmath>

void TopModule(int diagSize, double *matrixA) {
    #pragma HLS INTERFACE ap_none port=diagSize
    #pragma HLS INTERFACE ap_none port=matrixA
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Compute L₀₀ = √A₀₀
    double tmp = std::sqrt(matrixA[0]);
    matrixA[0] = tmp;

    // Normalize first column below the diagonal
    for (int i = 1; i < diagSize; i++) {
        matrixA[i*diagSize] /= tmp;
    }

    for (int j = 1; j < diagSize; j++) {
        // Compute dot-product of row j up to j–1 for the squared sum
        double sum = 0;
        for (int k = 0; k < j; k++) {
            #pragma HLS PIPELINE II=1
            double v = matrixA[j*diagSize + k];
            sum += v * v;
        }
        // Diagonal element Lⱼⱼ = sqrt(Aⱼⱼ – sum)
        double diag = std::sqrt(matrixA[j*diagSize + j] - sum);
        matrixA[j*diagSize + j] = diag;

        // For each row i = j+1…N–1, compute Lᵢⱼ
        for (int i = j+1; i < diagSize; i++) {
            double sum2 = 0;
            for (int k = 0; k < j; k++) {
                #pragma HLS PIPELINE II=1
                sum2 += matrixA[i*diagSize + k] * matrixA[j*diagSize + k];
            }
            matrixA[i*diagSize + j] = (matrixA[i*diagSize + j] - sum2) / diag;
        }
    }
}
