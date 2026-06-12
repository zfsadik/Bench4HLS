#include "ap_int.h"

void TopModule(bool load, ap_uint<512> data, ap_uint<512> & q) {
    #pragma HLS INTERFACE ap_none port=load
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=q
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<512> state = 0;

    if (load) {
        state = data;
    } else {
        ap_uint<512> X = state;
        ap_uint<512> term1 = ( (X >> 1) & X & (X << 1) );
        ap_uint<512> term2 = ( (~(X >> 1)) & (~X) & (~(X << 1)) );
        ap_uint<512> term3 = ( (X >> 1) & (~X) & (~(X << 1)) );
        state = ~(term1 | term2 | term3);
    }
    q = state;
}
