// fft_hls.cpp

#define SIZE 8            // must be a power of two
#define M    3            // log2(SIZE)
typedef float DTYPE;

void TopModule(DTYPE XR[SIZE], DTYPE XI[SIZE]) {
    #pragma HLS INTERFACE ap_none port=XR
    #pragma HLS INTERFACE ap_none port=XI
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS ARRAY_PARTITION variable=XR complete dim=1
    #pragma HLS ARRAY_PARTITION variable=XI complete dim=1

    // Pre-computed twiddle factors: cos/sin of (-2*pi/DFTpts * j)
    const DTYPE TW_COS[3][4] = {
        {1.0f,  0.0f,         0.0f,  0.0f},
        {1.0f,  0.0f,         0.0f,  0.0f},
        {1.0f,  0.70710678f,  0.0f, -0.70710678f}
    };
    const DTYPE TW_SIN[3][4] = {
        {0.0f,  0.0f,          0.0f,  0.0f},
        {0.0f, -1.0f,          0.0f,  0.0f},
        {0.0f, -0.70710678f,  -1.0f, -0.70710678f}
    };

    // --- Bit-reversal permutation ---
    {
        DTYPE tmpR, tmpI;
        tmpR = XR[1]; tmpI = XI[1]; XR[1] = XR[4]; XI[1] = XI[4]; XR[4] = tmpR; XI[4] = tmpI;
        tmpR = XR[3]; tmpI = XI[3]; XR[3] = XR[6]; XI[3] = XI[6]; XR[6] = tmpR; XI[6] = tmpI;
    }

    // --- Cooley-Tukey butterflies ---
    for (int stage = 1; stage <= M; stage++) {
        #pragma HLS UNROLL
        int DFTpts = 1 << stage;      // points in this sub-DFT
        int numBF  = DFTpts >> 1;     // butterflies per group
        for (int j = 0; j < numBF; j++) {
            #pragma HLS UNROLL
            DTYPE c = TW_COS[stage-1][j];
            DTYPE s = TW_SIN[stage-1][j];
            for (int i = j; i < SIZE; i += DFTpts) {
                #pragma HLS UNROLL
                int low = i + numBF;
                DTYPE tR = XR[low] * c - XI[low] * s;
                DTYPE tI = XI[low] * c + XR[low] * s;
                XR[low] = XR[i] - tR;
                XI[low] = XI[i] - tI;
                XR[i]   = XR[i] + tR;
                XI[i]   = XI[i] + tI;
            }
        }
    }
}
