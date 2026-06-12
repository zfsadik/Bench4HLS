#include "ap_int.h"

void TopModule(bool load, ap_uint<512> data, ap_uint<512> &q) {
#pragma HLS INTERFACE ap_none port=load
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=q
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<512> q_reg = 0;

    if (load) {
        q_reg = data;
    } else {
        ap_uint<511> X = q_reg.range(511, 1);
        ap_uint<511> Y = q_reg.range(510, 0);
        ap_uint<512> X_ext = X;
        ap_uint<512> Y_ext = ((ap_uint<512>)Y) << 1;
        q_reg = X_ext ^ Y_ext;
    }
    q = q_reg;
}
