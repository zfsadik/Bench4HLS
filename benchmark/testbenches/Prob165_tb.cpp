#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

#define HEIGHT 16
#define WIDTH  16

void TopModule(ap_uint<8> in[HEIGHT][WIDTH],
               ap_uint<8> out1[HEIGHT/2][WIDTH/2],
               ap_uint<8> out2[HEIGHT/4][WIDTH/4],
               ap_uint<8> out3[HEIGHT/8][WIDTH/8],
               ap_uint<8> out4[HEIGHT/16][WIDTH/16]);

// Reference helper 
static int clamp(int v, int maxv) {
    return (v < 0 ? 0 : (v >= maxv ? maxv - 1 : v));
}

void ref_pyr(ap_uint<8> in[HEIGHT][WIDTH],
             ap_uint<8> r1[HEIGHT/2][WIDTH/2],
             ap_uint<8> r2[HEIGHT/4][WIDTH/4],
             ap_uint<8> r3[HEIGHT/8][WIDTH/8],
             ap_uint<8> r4[HEIGHT/16][WIDTH/16]) {
    const int k[3][3] = {{1,2,1},
                         {2,4,2},
                         {1,2,1}};

    // Level1
    for (int y = 0; y < HEIGHT/2; ++y)
      for (int x = 0; x < WIDTH/2; ++x) {
        int sum = 0;
        for (int r = 0; r < 3; ++r)
          for (int c = 0; c < 3; ++c) {
            int yy = clamp(2*y + r - 1, HEIGHT);
            int xx = clamp(2*x + c - 1, WIDTH);
            sum += k[r][c] * in[yy][xx];
          }
        r1[y][x] = sum >> 4;
      }

    // Level2
    for (int y = 0; y < HEIGHT/4; ++y)
      for (int x = 0; x < WIDTH/4; ++x) {
        int sum = 0;
        for (int r = 0; r < 3; ++r)
          for (int c = 0; c < 3; ++c) {
            int yy = clamp(2*y + r - 1, HEIGHT/2);
            int xx = clamp(2*x + c - 1, WIDTH/2);
            sum += k[r][c] * r1[yy][xx];
          }
        r2[y][x] = sum >> 4;
      }

    // Level3
    for (int y = 0; y < HEIGHT/8; ++y)
      for (int x = 0; x < WIDTH/8; ++x) {
        int sum = 0;
        for (int r = 0; r < 3; ++r)
          for (int c = 0; c < 3; ++c) {
            int yy = clamp(2*y + r - 1, HEIGHT/4);
            int xx = clamp(2*x + c - 1, WIDTH/4);
            sum += k[r][c] * r2[yy][xx];
          }
        r3[y][x] = sum >> 4;
      }

    // Level4
    for (int y = 0; y < HEIGHT/16; ++y)
      for (int x = 0; x < WIDTH/16; ++x) {
        int sum = 0;
        for (int r = 0; r < 3; ++r)
          for (int c = 0; c < 3; ++c) {
            int yy = clamp(2*y + r - 1, HEIGHT/8);
            int xx = clamp(2*x + c - 1, WIDTH/8);
            sum += k[r][c] * r3[yy][xx];
          }
        r4[y][x] = sum >> 4;
      }
}

int main() {
    std::srand((unsigned)std::time(0));

    // Random input
    static ap_uint<8> in [HEIGHT][WIDTH];
    for (int y = 0; y < HEIGHT; ++y)
      for (int x = 0; x < WIDTH; ++x)
        in[y][x] = std::rand() & 0xFF;

    // DUT outputs
    static ap_uint<8> out1[HEIGHT/2][WIDTH/2],
                       out2[HEIGHT/4][WIDTH/4],
                       out3[HEIGHT/8][WIDTH/8],
                       out4[HEIGHT/16][WIDTH/16];

    // Reference outputs
    static ap_uint<8> r1[HEIGHT/2][WIDTH/2],
                       r2[HEIGHT/4][WIDTH/4],
                       r3[HEIGHT/8][WIDTH/8],
                       r4[HEIGHT/16][WIDTH/16];

    // Invoke design
    TopModule(in, out1, out2, out3, out4);

    // Compute reference
    ref_pyr(in, r1, r2, r3, r4);

    // Compare
    int cycleCount = 0, errorCount = 0;

    // Level 1
    for (int y = 0; y < HEIGHT/2; ++y)
    for (int x = 0; x < WIDTH/2; ++x) {
        if (out1[y][x] != r1[y][x]) {
            std::cout << "Error at test case " << cycleCount
                      << ": level=1 coords(" << y << "," << x << ")"
                      << " expected =" << (unsigned)r1[y][x]
                      << ", got="   << (unsigned)out1[y][x]
                      << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Level 2
    for (int y = 0; y < HEIGHT/4; ++y)
    for (int x = 0; x < WIDTH/4; ++x) {
        if (out2[y][x] != r2[y][x]) {
            std::cout << "Error at test case " << cycleCount
                      << ": level=2 coords(" << y << "," << x << ")"
                      << " expected =" << (unsigned)r2[y][x]
                      << ", got="   << (unsigned)out2[y][x]
                      << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Level 3
    for (int y = 0; y < HEIGHT/8; ++y)
    for (int x = 0; x < WIDTH/8; ++x) {
        if (out3[y][x] != r3[y][x]) {
            std::cout << "Error at test case " << cycleCount
                      << ": level=3 coords(" << y << "," << x << ")"
                      << " expected =" << (unsigned)r3[y][x]
                      << ", got="   << (unsigned)out3[y][x]
                      << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Level 4
    for (int y = 0; y < HEIGHT/16; ++y)
    for (int x = 0; x < WIDTH/16; ++x) {
        if (out4[y][x] != r4[y][x]) {
            std::cout << "Error at test case " << cycleCount
                      << ": level=4 coords(" << y << "," << x << ")"
                      << " expected =" << (unsigned)r4[y][x]
                      << ", got="   << (unsigned)out4[y][x]
                      << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount 
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " 
                  << cycleCount << " cases." << std::endl;
    }
    return 0;
}
