#include <ap_int.h>

void TopModule(
    bool         resetn,
    ap_uint<3>   r,
    ap_uint<3> & g
) {
    #pragma HLS INTERFACE ap_none port=resetn
    #pragma HLS INTERFACE ap_none port=r
    #pragma HLS INTERFACE ap_none port=g
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    enum States { A=0, B=1, C=2, D=3 };
    static States state = A;
    #pragma HLS RESET variable=state

    g = 0;
    if (state == B) g[0] = 1;
    if (state == C) g[1] = 1;
    if (state == D) g[2] = 1;

    if (!resetn) {
        state = A;
    } else {
        switch (state) {
          case A:
            if (r[0])      state = B;
            else if (r[1]) state = C;
            else if (r[2]) state = D;
            else           state = A;
            break;
          case B:
            state = r[0] ? B : A;
            break;
          case C:
            state = r[1] ? C : A;
            break;
          case D:
            state = r[2] ? D : A;
            break;
        }
    }
}
