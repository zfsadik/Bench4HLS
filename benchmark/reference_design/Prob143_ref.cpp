#include <ap_int.h>

void TopModule(
    bool        reset,
    ap_uint<3>  s,
    bool       &fr3,
    bool       &fr2,
    bool       &fr1,
    bool       &dfr
) {
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=s
    #pragma HLS INTERFACE ap_none port=fr3
    #pragma HLS INTERFACE ap_none port=fr2
    #pragma HLS INTERFACE ap_none port=fr1
    #pragma HLS INTERFACE ap_none port=dfr
    #pragma HLS PIPELINE II=1

    enum States { A2=0, B1=1, B2=2, C1=3, C2=4, D1=5 };
    static States state = A2;
    #pragma HLS RESET variable=state

    switch (state) {
      case A2: fr3=1; fr2=1; fr1=1; dfr=1; break;
      case B1: fr3=0; fr2=1; fr1=1; dfr=0; break;
      case B2: fr3=0; fr2=1; fr1=1; dfr=1; break;
      case C1: fr3=0; fr2=0; fr1=1; dfr=0; break;
      case C2: fr3=0; fr2=0; fr1=1; dfr=1; break;
      case D1: fr3=0; fr2=0; fr1=0; dfr=0; break;
      default: fr3=0; fr2=0; fr1=0; dfr=0; break;
    }

    if (reset) {
        state = A2;
    } else {
        States next = state;
        switch (state) {
          case A2:
            if (s[0])       next = B1;
            else            next = A2;
            break;
          case B1:
            if (s[1])       next = C1;
            else if (s[0])  next = B1;
            else            next = A2;
            break;
          case B2:
            if (s[1])       next = C1;
            else if (s[0])  next = B2;
            else            next = A2;
            break;
          case C1:
            if (s[2])       next = D1;
            else if (s[1])  next = C1;
            else            next = B2;
            break;
          case C2:
            if (s[2])       next = D1;
            else if (s[1])  next = C2;
            else            next = B2;
            break;
          case D1:
            if (s[2])       next = D1;
            else            next = C2;
            break;
        }
        state = next;
    }
}
