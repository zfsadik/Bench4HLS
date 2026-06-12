#include <cmath>

void TopModule(double float_n,
               double data[100][80],
               double corr[80][80],
               double mean[80],
               double stddev[80])
{
    #pragma HLS INTERFACE ap_none port=float_n
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=corr
    #pragma HLS INTERFACE ap_none port=mean
    #pragma HLS INTERFACE ap_none port=stddev
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS ARRAY_PARTITION variable=data cyclic factor=4 dim=2
    #pragma HLS ARRAY_PARTITION variable=data cyclic factor=4 dim=1

    const double eps = 0.1;

    // 1) Mean
    for (int j = 0; j < 80; ++j) {
        double sum = 0.0;
        for (int i = 0; i < 100; ++i) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=4
            sum += data[i][j];
        }
        mean[j] = sum / float_n;
    }

    // 2) Standard Deviation (with non-zero guard)
    for (int j = 0; j < 80; ++j) {
        double sumsq = 0.0;
        for (int i = 0; i < 100; ++i) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=4
            double d = data[i][j] - mean[j];
            sumsq += d * d;
        }
        double var = sumsq / float_n;
        double s   = std::sqrt(var);
        stddev[j]  = (s <= eps ? 1.0 : s);
    }

    // 3) Center & scale
    double norm_factor = std::sqrt(float_n);
    double inv_scale[80];
    for (int j = 0; j < 80; ++j) {
        #pragma HLS PIPELINE II=1
        inv_scale[j] = 1.0 / (norm_factor * stddev[j]);
    }
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 80; ++j) {
            #pragma HLS PIPELINE II=1
            data[i][j] = (data[i][j] - mean[j]) * inv_scale[j];
        }
    }

    // 4) Correlation matrix
    for (int i = 0; i < 80; ++i) {
        corr[i][i] = 1.0;
        for (int j = i + 1; j < 80; ++j) {
            double sum = 0.0;
            for (int k = 0; k < 100; ++k) {
                #pragma HLS PIPELINE II=1
                #pragma HLS UNROLL factor=4
                sum += data[k][i] * data[k][j];
            }
            corr[i][j] = sum;
            corr[j][i] = sum;
        }
    }
}
