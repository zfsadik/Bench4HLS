void TopModule(const int input[8][8], int output[8][8]) {
#pragma HLS INTERFACE ap_none port=input
#pragma HLS INTERFACE ap_none port=output
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS ARRAY_PARTITION variable=input complete dim=0
    constexpr int IMG_H = 8;
    constexpr int IMG_W = 8;
    constexpr int K     = 5;

    RowLoop: for (int i = 0; i < IMG_H; i++) {
        ColLoop: for (int j = 0; j < IMG_W; j++) {
#pragma HLS PIPELINE II=1
            int sum = 0;

            for (int ki = 0; ki < K; ki++) {
#pragma HLS UNROLL
                for (int kj = 0; kj < K; kj++) {
#pragma HLS UNROLL
                    int ii = i + ki - (K/2);
                    int jj = j + kj - (K/2);
                    if (ii >= 0 && ii < IMG_H && jj >= 0 && jj < IMG_W) {
                        sum += input[ii][jj];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
}
