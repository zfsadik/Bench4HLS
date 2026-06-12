#include <complex>

void TopModule(std::complex<float> x[64]) {
#pragma HLS INTERFACE ap_none port=x
#pragma HLS INTERFACE ap_ctrl_none port=return

    // Bit‑reversal permutation
    static const int br[64] = {
         0, 32, 16, 48,  8, 40, 24, 56,  4, 36, 20, 52, 12, 44, 28, 60,
         2, 34, 18, 50, 10, 42, 26, 58,  6, 38, 22, 54, 14, 46, 30, 62,
         1, 33, 17, 49,  9, 41, 25, 57,  5, 37, 21, 53, 13, 45, 29, 61,
         3, 35, 19, 51, 11, 43, 27, 59,  7, 39, 23, 55, 15, 47, 31, 63
    };

    static const std::complex<float> W[32] = {
        { 1.000000f,  0.000000f}, { 0.995185f, -0.098017f},
        { 0.980785f, -0.195090f}, { 0.956940f, -0.290285f},
        { 0.923880f, -0.382683f}, { 0.881921f, -0.471397f},
        { 0.831470f, -0.555570f}, { 0.773010f, -0.634393f},
        { 0.707107f, -0.707107f}, { 0.634393f, -0.773010f},
        { 0.555570f, -0.831470f}, { 0.471397f, -0.881921f},
        { 0.382683f, -0.923880f}, { 0.290285f, -0.956940f},
        { 0.195090f, -0.980785f}, { 0.098017f, -0.995185f},
        { 0.000000f, -1.000000f}, {-0.098017f, -0.995185f},
        {-0.195090f, -0.980785f}, {-0.290285f, -0.956940f},
        {-0.382683f, -0.923880f}, {-0.471397f, -0.881921f},
        {-0.555570f, -0.831470f}, {-0.634393f, -0.773010f},
        {-0.707107f, -0.707107f}, {-0.773010f, -0.634393f},
        {-0.831470f, -0.555570f}, {-0.881921f, -0.471397f},
        {-0.923880f, -0.382683f}, {-0.956940f, -0.290285f},
        {-0.980785f, -0.195090f}, {-0.995185f, -0.098017f}
    };

    for (int i = 0; i < 64; i++) {
        if (i < br[i]) {
            auto tmp = x[i];
            x[i] = x[br[i]];
            x[br[i]] = tmp;
        }
    }

    // Cooley–Tukey radix‑2 decimation‑in‑time FFT
    for (int len = 2; len <= 64; len <<= 1) {
        int half = len >> 1;
        int stride = 32 / half;
        for (int i = 0; i < 64; i += len) {
            for (int k = 0; k < half; k++) {
#pragma HLS UNROLL factor=4
                std::complex<float> tw = W[k * stride];
                auto u = x[i + k];
                auto v = x[i + k + half];
                // manual "complex multiply" by twiddle (c + j·s)
                std::complex<float> t{ v.real()*tw.real() - v.imag()*tw.imag(),
                                       v.real()*tw.imag() + v.imag()*tw.real() };
                x[i + k] = u + t;
                x[i + k + half] = u - t;
            }
        }
    }
}
