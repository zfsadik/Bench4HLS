Design a module for HLS that implements a 2‑D convolution over an 8×8 image using a fixed 5×5 kernel of all ones. For each of the 8×8 output pixels, the design slides a 5×5 window centered at that pixel (treating out‑of‑bounds positions as zero), multiplies every valid sample in that window by the corresponding kernel entry (all ones), sums the results, and writes the sum to the corresponding position in the output. The input array is fully partitioned into individual registers, allowing all elements to be accessed simultaneously; with the inner kernel loops unrolled, the design achieves II=1 on the outer pixel-scan loops.

The top-level function should have the following prototype:
void TopModule(const int input[8][8], int output[8][8])
