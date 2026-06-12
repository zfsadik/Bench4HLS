#include <ap_int.h>

void TopModule(ap_uint<1> in, ap_uint<1> reset, ap_uint<8> &out_byte, ap_uint<1> &done) {
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=out_byte
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // State encoding
    enum States { B0=0, B1=1, B2=2, B3=3, B4=4, B5=5, B6=6, B7=7,
                  START=8, STOP=9, DONE=10, ERR=11 };
    static States      state     = START;
    static ap_uint<10> shift_reg = 0;
    #pragma HLS RESET variable=state
    #pragma HLS RESET variable=shift_reg

    if (reset) {
        state     = START;
        shift_reg = 0;
    } else {
        States next;
        switch (state) {
          case START: next = in ? START : B0;   break;
          case B0:    next = B1;               break;
          case B1:    next = B2;               break;
          case B2:    next = B3;               break;
          case B3:    next = B4;               break;
          case B4:    next = B5;               break;
          case B5:    next = B6;               break;
          case B6:    next = B7;               break;
          case B7:    next = STOP;             break;
          case STOP:  next = in ? DONE : ERR;  break;
          case DONE:  next = in ? START : B0;  break;
          case ERR:   next = in ? START : ERR; break;
          default:    next = START;            break;
        }
        state     = next;
        shift_reg = (shift_reg >> 1) | (ap_uint<10>(in) << 9);
    }

    done     = (state == DONE);
    out_byte = shift_reg.range(8,1);
}
