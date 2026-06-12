#include "ap_int.h"
#define WIDTH 8

inline ap_uint<WIDTH> mont(ap_uint<WIDTH> x, ap_uint<WIDTH> y,
                           ap_uint<WIDTH> m, ap_uint<WIDTH> mprime) {
    ap_uint<2*WIDTH> T  = (ap_uint<2*WIDTH>)x * y;
    ap_uint<WIDTH>   q  = (T * mprime) & ap_uint<WIDTH>(-1);
    ap_uint<2*WIDTH> Mq = (ap_uint<2*WIDTH>)q * m;
    ap_uint<2*WIDTH> U  = (T + Mq) >> WIDTH;
    return U >= m ? ap_uint<WIDTH>(U - m) : ap_uint<WIDTH>(U);
}

void TopModule(
    ap_uint<WIDTH> a,
    ap_uint<WIDTH> b,
    ap_uint<WIDTH> m,
    ap_uint<WIDTH> mprime,
    ap_uint<WIDTH> &result) {
    #pragma HLS INTERFACE ap_none port=a
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=m
    #pragma HLS INTERFACE ap_none port=mprime
    #pragma HLS INTERFACE ap_none port=result
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ap_uint<WIDTH> R_mod = ((ap_uint<2*WIDTH>(1) << WIDTH) % m);

    ap_uint<WIDTH> aM   = ((ap_uint<2*WIDTH>)a * R_mod) % m;
    ap_uint<WIDTH> resM = ((ap_uint<2*WIDTH>)1 * R_mod) % m;

    for (int i = WIDTH - 1; i >= 0; --i) {
        #pragma HLS UNROLL
        resM = mont(resM, resM, m, mprime);
        if (b[i]) {
            resM = mont(resM, aM, m, mprime);
        }
    }

    result = mont(resM, ap_uint<WIDTH>(1), m, mprime);
}
