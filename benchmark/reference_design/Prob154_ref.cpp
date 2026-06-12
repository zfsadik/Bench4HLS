void TopModule(int* in1, int* in2, int* out_r, int size, int rep_count) {
#pragma HLS INTERFACE m_axi     port=in1     depth=256
#pragma HLS INTERFACE m_axi     port=in2     depth=256
#pragma HLS INTERFACE m_axi     port=out_r   depth=256
#pragma HLS INTERFACE ap_none   port=size
#pragma HLS INTERFACE ap_none   port=rep_count
#pragma HLS INTERFACE ap_ctrl_none port=return

    // Local buffers
    int A[16][16];
    int B[16][16];
    int C[16][16];

#pragma HLS ARRAY_PARTITION variable=A dim=2 complete
#pragma HLS ARRAY_PARTITION variable=B dim=1 complete
#pragma HLS ARRAY_PARTITION variable=C dim=2 complete

    // Read in1 → A
    LOAD_A:
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
#pragma HLS PIPELINE II=1
            A[i][j] = in1[i * size + j];
        }
    }

    // Read in2 → B
    LOAD_B:
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
#pragma HLS PIPELINE II=1
            B[i][j] = in2[i * size + j];
        }
    }

    // Perform rep_count rounds of block‑matrix multiply
    COMPUTE:
    for (int r = 0; r < rep_count; ++r) {
    row_loop:
        for (int row = 0; row < size; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
        k_loop:
            for (int k = 0; k < size; ++k) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
                int acc = 0;
            col_loop:
                for (int col = 0; col < size; ++col) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
#pragma HLS UNROLL
                    acc += A[row][col] * B[col][k];
                }
                C[row][k] = acc;
            }
        }
    }

    // Write C → out_r
    WRITE:
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16
#pragma HLS PIPELINE II=1
            out_r[i * size + j] = C[i][j];
        }
    }
}
