void TopModule(int tsteps,
               int n,
               double u[60][60],
               double v[60][60],
               double p[60][60],
               double q[60][60],
               bool   &valid) {
#pragma HLS INTERFACE ap_memory port=u
#pragma HLS INTERFACE ap_memory port=v
#pragma HLS INTERFACE ap_memory port=p
#pragma HLS INTERFACE ap_memory port=q
#pragma HLS INTERFACE ap_none   port=tsteps
#pragma HLS INTERFACE ap_none   port=n
#pragma HLS INTERFACE ap_none   port=valid
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS ARRAY_PARTITION variable=u cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=v cyclic factor=2 dim=1

    // Precompute constants
    double DX = 1.0 / n;
    double DY = 1.0 / n;
    double DT = 1.0 / tsteps;
    double B1 = 2.0, B2 = 1.0;
    double mul1 = B1 * DT / (DX * DX);
    double mul2 = B2 * DT / (DY * DY);
    double a = -mul1 * 0.5, b = 1.0 + mul1, c = a;
    double d = -mul2 * 0.5, e = 1.0 + mul2, f = d;

    // Main time‐stepping loop
    for (int t = 1; t <= tsteps; t++) {
        // Column sweep
        for (int i = 1; i < n-1; i++) {
            v[0][i] = 1.0;
            p[i][0] = 0.0;
            q[i][0] = v[0][i];
            // Forward solve
            for (int j = 1; j < n-1; j++) {
#pragma HLS PIPELINE II=1
                double denom = a * p[i][j-1] + b;
                double inv_denom = 1.0 / denom;
                p[i][j] = -c * inv_denom;
                q[i][j] = ( -d * u[j-1][i]
                            + (1.0+2.0*d) * u[j][i]
                            - f * u[j+1][i]
                            - a * q[i][j-1]
                          ) * inv_denom;
            }
            v[n-1][i] = 1.0;
            // Backward solve
            for (int jj = n-2; jj >= 1; jj--) {
#pragma HLS PIPELINE II=1
                v[jj][i] = p[i][jj] * v[jj+1][i] + q[i][jj];
            }
        }
        // Row sweep
        for (int i = 1; i < n-1; i++) {
            u[i][0] = 1.0;
            p[i][0] = 0.0;
            q[i][0] = u[i][0];
            // Forward solve
            for (int j = 1; j < n-1; j++) {
#pragma HLS PIPELINE II=1
                double denom = d * p[i][j-1] + e;
                double inv_denom = 1.0 / denom;
                p[i][j] = -f * inv_denom;
                q[i][j] = ( -a * v[i][j-1]
                            + (1.0+2.0*a) * v[i][j]
                            - c * v[i][j+1]
                            - d * q[i][j-1]
                          ) * inv_denom;
            }
            u[i][n-1] = 1.0;
            // Backward solve
            for (int jj = n-2; jj >= 1; jj--) {
#pragma HLS PIPELINE II=1
                u[i][jj] = p[i][jj] * u[i][jj+1] + q[i][jj];
            }
        }
    }

    valid = true;
}
