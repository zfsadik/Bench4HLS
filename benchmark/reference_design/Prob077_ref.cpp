#include "ap_int.h"

void TopModule(bool load, ap_uint<10> data, bool &tc) {
    #pragma HLS INTERFACE ap_none port=load
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=tc
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static ap_uint<10> count_value = 0;

    if (load) {
        count_value = data;
    } else if (count_value != 0) {
        count_value = count_value - 1;
    }

    tc = (count_value == 0);
}
