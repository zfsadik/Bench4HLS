Write an HLS code that implements a 64‑point in‑place radix‑2 decimation‑in‑time FFT in two distinct phases. First, it performs a bit‑reversal permutation on the input array. In hardware terms, this is a fixed, combinational addr­ess‑swapping network that reorders the 6‑bit address of each sample so that the natural index map is reversed bit‑wise.

Once the data are in bit‑reversed order, the design executes six sequential butterfly stages (since 2⁶ = 64), each halving the processing "distance" between paired samples. At stage m (m from 1 to 6), the array is divided into groups of 2ᵐ points. Within each group, pairs of values separated by 2ᵐ⁻¹ indices are combined by adding and subtracting a rotated version of the second sample—the so‑called "twiddle" multiplication by e^(–j2πk/2ᵐ). Conceptually, each butterfly computes:

	the sum (u + v·W)
	the difference (u – v·W)

where W is the complex weight corresponding to the position k within the group. In hardware, each stage can be fully pipelined with an initiation interval of 1 and unrolled to process multiple butterflies in parallel, trading off resource usage and latency.

Throughout, twiddle factors (cosine and sine terms) are generated on‑the‑fly in each stage by computing angle = –2π·k/length and then using CORDIC or lookup logic to produce the real and imaginary multipliers.

The top-level function should have the following prototype:
void TopModule(std::complex<float> x[64])