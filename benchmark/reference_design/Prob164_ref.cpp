#include <cmath>

#define UNFACT    32
#define MAX_ROW   32
#define MAX_COL   32

typedef float float32_t;

// Complex multiply
static inline void cmult(float32_t a, float32_t b,
                         float32_t c, float32_t d,
                         float32_t &R, float32_t &I) {
    #pragma HLS INLINE
    R = a*c - b*d;
    I = a*d + b*c;
}

// Complex divide
static inline void cdiv(float32_t a, float32_t b,
                        float32_t c, float32_t d,
                        float32_t &R, float32_t &I) {
    #pragma HLS INLINE
    float32_t denom = c*c + d*d;
    R = (a*c + b*d) / denom;
    I = (b*c - a*d) / denom;
}

void TopModule(float32_t Qr_i[MAX_ROW*MAX_COL],
               float32_t Qi_i[MAX_ROW*MAX_COL],
               int col, int row,
               float32_t br_i[MAX_ROW],
               float32_t bi_i[MAX_ROW],
               float32_t xr_o[MAX_COL],
               float32_t xi_o[MAX_COL])
{
    #pragma HLS INTERFACE ap_none port=Qr_i
    #pragma HLS INTERFACE ap_none port=Qi_i
    #pragma HLS INTERFACE ap_none port=br_i
    #pragma HLS INTERFACE ap_none port=bi_i
    #pragma HLS INTERFACE ap_none port=xr_o
    #pragma HLS INTERFACE ap_none port=xi_o
    #pragma HLS INTERFACE ap_none port=col
    #pragma HLS INTERFACE ap_none port=row
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Local on-chip storage
    float32_t Qr[MAX_ROW][MAX_COL];
    float32_t Qi[MAX_ROW][MAX_COL];
    float32_t br[MAX_ROW], bi[MAX_ROW];
    float32_t xr[MAX_COL], xi[MAX_COL];
    float32_t Rr[MAX_ROW][MAX_ROW], Ri[MAX_ROW][MAX_ROW];

    // Partition arrays to maximize parallel access
    #pragma HLS ARRAY_PARTITION variable=Qr complete dim=1
    #pragma HLS ARRAY_PARTITION variable=Qi complete dim=1
    #pragma HLS ARRAY_PARTITION variable=br complete dim=1
    #pragma HLS ARRAY_PARTITION variable=bi complete dim=1
    #pragma HLS ARRAY_PARTITION variable=xr complete dim=1
    #pragma HLS ARRAY_PARTITION variable=xi complete dim=1
    #pragma HLS ARRAY_PARTITION variable=Rr complete dim=1
    #pragma HLS ARRAY_PARTITION variable=Ri complete dim=1

    // Scratch vectors for accumulation
    float32_t Qcr[MAX_ROW], Qci[MAX_ROW];
    #pragma HLS ARRAY_PARTITION variable=Qcr complete dim=1
    #pragma HLS ARRAY_PARTITION variable=Qci complete dim=1
    float32_t rtr[UNFACT], rti[UNFACT];
    float32_t nrmr[UNFACT], nrmi[UNFACT];
    float32_t tr, ti, acr, aci;

    // 1) Load global inputs into local on-chip arrays
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            #pragma HLS PIPELINE II=1
            Qr[i][j] = Qr_i[i*col + j];
            Qi[i][j] = Qi_i[i*col + j];
        }
    }
    for (int i = 0; i < row; i++) {
        #pragma HLS PIPELINE II=1
        br[i] = br_i[i];
        bi[i] = bi_i[i];
    }

    // 2) Modified Gram–Schmidt QR decomposition
    for (int ii = 0; ii < col; ii++) {
        // copy column ii
        for (int k = 0; k < row; k++) {
            #pragma HLS PIPELINE II=1
            Qcr[k] = Qr[k][ii];
            Qci[k] = Qi[k][ii];
        }

        // subtract projections onto earlier Q columns
        for (int j = 0; j < ii; j++) {
            // zero accumulators
            for (int u = 0; u < UNFACT; u++) {
                #pragma HLS UNROLL
                rtr[u] = rti[u] = 0;
            }
            // inner product
            for (int k = 0; k < row; k++) {
                #pragma HLS PIPELINE II=1
                cmult(Qr[k][j], -Qi[k][j], Qcr[k], Qci[k], tr, ti);
                rtr[k % UNFACT] += tr;
                rti[k % UNFACT] += ti;
            }
            // reduce
            for (int u = 1; u < UNFACT; u++) {
                #pragma HLS UNROLL
                rtr[0] += rtr[u];
                rti[0] += rti[u];
            }
            Rr[j][ii] = rtr[0];
            Ri[j][ii] = rti[0];

            // subtract projection
            for (int k = 0; k < row; k++) {
                #pragma HLS PIPELINE II=1
                cmult(Qr[k][j], Qi[k][j], rtr[0], rti[0], tr, ti);
                Qcr[k] -= tr;
                Qci[k] -= ti;
            }
        }

        // compute norm
        for (int u = 0; u < UNFACT; u++) {
            #pragma HLS UNROLL
            nrmr[u] = nrmi[u] = 0;
        }
        for (int k = 0; k < row; k++) {
            #pragma HLS PIPELINE II=1
            nrmr[k % UNFACT] += Qcr[k]*Qcr[k];
            nrmi[k % UNFACT] += Qci[k]*Qci[k];
        }
        for (int u = 1; u < UNFACT; u++) {
            #pragma HLS UNROLL
            nrmr[0] += nrmr[u];
            nrmi[0] += nrmi[u];
        }
        Rr[ii][ii] = std::sqrt(nrmr[0] + nrmi[0]);
        Ri[ii][ii] = 0.0f;

        // normalize Q
        for (int k = 0; k < row; k++) {
            #pragma HLS PIPELINE II=1
            Qr[k][ii] = Qcr[k] / Rr[ii][ii];
            Qi[k][ii] = Qci[k] / Rr[ii][ii];
        }
    }

    // 3) Forming the Transformed RHS: compute Qᴴ·b
    float32_t Qrtb[MAX_ROW], Qitb[MAX_ROW];
    for (int i = 0; i < col; i++) {
        for (int u = 0; u < UNFACT; u++) {
            #pragma HLS UNROLL
            rtr[u] = rti[u] = 0;
        }
        for (int k = 0; k < row; k++) {
            #pragma HLS PIPELINE II=1
            cmult(Qr[k][i], -Qi[k][i], br[k], bi[k], tr, ti);
            rtr[k % UNFACT] += tr;
            rti[k % UNFACT] += ti;
        }
        for (int u = 1; u < UNFACT; u++) {
            #pragma HLS UNROLL
            rtr[0] += rtr[u];
            rti[0] += rti[u];
        }
        Qrtb[i] = rtr[0];
        Qitb[i] = rti[0];
    }

    // 4) Solve R·x = Qᴴ·b by back-substitution
    for (int j = col-1; j >= 0; j--) {
        float32_t sum_r = 0.0f, sum_i = 0.0f;
        for (int k = j+1; k < col; k++) {
            #pragma HLS PIPELINE II=1
            cmult(Rr[j][k], Ri[j][k], xr[k], xi[k], acr, aci);
            sum_r += acr;
            sum_i += aci;
        }
        cdiv(Qrtb[j] - sum_r, Qitb[j] - sum_i,
             Rr[j][j],    Ri[j][j],
             xr[j],       xi[j]);
    }

    // 5) Stream out the result
    for (int i = 0; i < col; i++) {
        #pragma HLS PIPELINE II=1
        xr_o[i] = xr[i];
        xi_o[i] = xi[i];
    }
}
