Design a four-stage Gaussian pyramid generator on a 16×16 input image, producing successively smaller, blurred versions at 8×8, 4×4, 2×2, and 1×1 resolutions. At each stage it performs two operations in lock-step:
	• 3×3 Gaussian blur on every 2×2 block of the previous level, using clamped edge extension.
	• Down-sampling by 2 in both dimensions.

Process Flow:
1. Input: a 16×16 image buffer.

2. Kernel: use a fixed 3×3 Gaussian weight matrix [1 2 1; 2 4 2; 1 2 1] normalized by dividing the nine-sum by 16.

3. Repeat for four levels:
	• Blur: for each output pixel, center the 3×3 window on pixel (2y, 2x) of the current image (clamping edges), multiply–accumulate with the kernel, then shift right by 4.
	• Downsample: keep only every second row and column (i.e. output(y,x) uses input(2y,2x) neighborhood).

4. Outputs: 8×8 at level 1, 4×4 at level 2, 2×2 at level 3, and 1×1 at level 4.

The top-level function should have the following prototype:
void TopModule(ap_uint<8> in[HEIGHT][WIDTH],
               ap_uint<8> out1[HEIGHT/2][WIDTH/2],
               ap_uint<8> out2[HEIGHT/4][WIDTH/4],
               ap_uint<8> out3[HEIGHT/8][WIDTH/8],
               ap_uint<8> out4[HEIGHT/16][WIDTH/16])

#define HEIGHT 16
#define WIDTH  16
