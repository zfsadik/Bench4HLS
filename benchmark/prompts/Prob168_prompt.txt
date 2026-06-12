Implement a four stage statistical correlation kernel over a 100×80 data matrix in HLS. The kernel follows these simple four-step pipeline over an M×N data matrix (here 100×80):

	1. Mean: Sum each column's M samples and divide by the length (float_n) to get an N element mean vector.
	2. Std Dev with Guard: Compute each column's variance (mean-centered squared sum ÷ length), take the square root, and enforce a small minimum to avoid zeros.  This produces an 80-element stddev vector
	3. Normalize: Subtract the column mean and divide by (√length × std dev) so every column has zero mean and unit variance.
	4. Correlation: Form the N×N symmetric matrix (corr) by setting the diagonal to 1 and filling each off-diagonal entry with the dot product of the two normalized columns.

The top-level function should have the following prototype:
void TopModule(double float_n,
               double data[100][80],
               double corr[80][80],
               double mean[80],
               double stddev[80])
