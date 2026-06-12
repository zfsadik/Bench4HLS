#include <ap_int.h>

void TopModule(
    ap_uint<8>  in,
    bool        reset,
    ap_uint<24> &out_bytes,
    bool        &done
) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=out_bytes
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    enum States { BYTE1 = 0, BYTE2, BYTE3, DONE };
    static States      state = BYTE1;
    static ap_uint<24> buf   = 0;

    if (reset) {
        state     = BYTE1;
        buf       = 0;
        done      = false;
        out_bytes = 0;
    } else {
        done      = (state == DONE);
        out_bytes = buf;

        States next = state;
        switch (state) {
          case BYTE1:
            next = (in[3] == 1) ? BYTE2 : BYTE1;
            break;
          case BYTE2:
            next = BYTE3;
            break;
          case BYTE3:
            next = DONE;
            break;
          case DONE:
            next = (in[3] == 1) ? BYTE2 : BYTE1;
            break;
        }

        if (next == BYTE2) buf.range(23, 16) = in;
        if (next == BYTE3) buf.range(15,  8) = in;
        if (next == DONE)  buf.range( 7,  0) = in;

        state = next;
    }
}
