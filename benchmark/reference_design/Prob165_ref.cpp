#include "ap_int.h"

#define HEIGHT 16
#define WIDTH  16

void TopModule(ap_uint<8> in[HEIGHT][WIDTH],
               ap_uint<8> out1[HEIGHT/2][WIDTH/2],
               ap_uint<8> out2[HEIGHT/4][WIDTH/4],
               ap_uint<8> out3[HEIGHT/8][WIDTH/8],
               ap_uint<8> out4[HEIGHT/16][WIDTH/16]) {
#pragma HLS INTERFACE ap_none port=in
#pragma HLS INTERFACE ap_none port=out1
#pragma HLS INTERFACE ap_none port=out2
#pragma HLS INTERFACE ap_none port=out3
#pragma HLS INTERFACE ap_none port=out4
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=out2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=out3 complete dim=0

    // 3×3 Gaussian kernel weights
    const int k[3][3] = {{1,2,1},
                         {2,4,2},
                         {1,2,1}};

    auto clamp = [](int v, int maxv) {
        return (v < 0 ? 0 : (v >= maxv ? maxv - 1 : v));
    };

    // Level 1: blur+downsample from in→out1
    for (int y = 0; y < HEIGHT/2; ++y) {
        for (int x = 0; x < WIDTH/2; ++x) {
#pragma HLS PIPELINE II=1
            int sum = 0;
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) {
                    int yy = clamp(2*y + r - 1, HEIGHT);
                    int xx = clamp(2*x + c - 1, WIDTH);
                    sum += k[r][c] * in[yy][xx];
                }
            out1[y][x] = sum >> 4;  // divide by 16
        }
    }

    // Level 2: from out1→out2
    for (int y = 0; y < HEIGHT/4; ++y) {
        for (int x = 0; x < WIDTH/4; ++x) {
#pragma HLS PIPELINE II=1
            int sum = 0;
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) {
                    int yy = clamp(2*y + r - 1, HEIGHT/2);
                    int xx = clamp(2*x + c - 1, WIDTH/2);
                    sum += k[r][c] * out1[yy][xx];
                }
            out2[y][x] = sum >> 4;
        }
    }

    // Level 3: from out2→out3
    for (int y = 0; y < HEIGHT/8; ++y) {
        for (int x = 0; x < WIDTH/8; ++x) {
#pragma HLS PIPELINE II=1
            int sum = 0;
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) {
                    int yy = clamp(2*y + r - 1, HEIGHT/4);
                    int xx = clamp(2*x + c - 1, WIDTH/4);
                    sum += k[r][c] * out2[yy][xx];
                }
            out3[y][x] = sum >> 4;
        }
    }

    // Level 4: from out3→out4
    for (int y = 0; y < HEIGHT/16; ++y) {
        for (int x = 0; x < WIDTH/16; ++x) {
#pragma HLS PIPELINE II=1
            int sum = 0;
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) {
                    int yy = clamp(2*y + r - 1, HEIGHT/8);
                    int xx = clamp(2*x + c - 1, WIDTH/8);
                    sum += k[r][c] * out3[yy][xx];
                }
            out4[y][x] = sum >> 4;
        }
    }
}
