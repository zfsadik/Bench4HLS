#include <ap_int.h>

enum StateEnum {
  S      = 0,
  S1     = 1,
  S11    = 2,
  S110   = 3,
  B0     = 4,
  B1     = 5,
  B2     = 6,
  B3     = 7,
  CountSt= 8,
  WaitSt = 9
};

void TopModule(
    bool reset,
    bool data,
    ap_uint<4> &count,
    bool &counting,
    bool &done,
    bool ack
) {
#pragma HLS INTERFACE ap_none port=reset
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=count
#pragma HLS INTERFACE ap_none port=counting
#pragma HLS INTERFACE ap_none port=done
#pragma HLS INTERFACE ap_none port=ack
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    // Registers
    static ap_uint<4> state   = S;
    static ap_uint<4> scount  = 0;    // shifted-in delay or remaining count
    static ap_uint<10> fcount = 0;    // fast 0–999 counter

    ap_uint<4> next = S;
    bool done_counting = (state == CountSt) &&
                         (fcount == 999) &&
                         (scount == 0);

    switch (state) {
      case S:       next = data     ? S1     : S;      break;
      case S1:      next = data     ? S11    : S;      break;
      case S11:     next = data     ? S11    : S110;   break;
      case S110:    next = data     ? B0     : S;      break;
      case B0:      next = B1;                         break;
      case B1:      next = B2;                         break;
      case B2:      next = B3;                         break;
      case B3:      next = CountSt;                    break;
      case CountSt: next = done_counting ? WaitSt : CountSt; break;
      case WaitSt:  next = ack      ? S      : WaitSt; break;
      default:      next = S;                          break;
    }

    // Moore outputs
    counting = (state == CountSt);
    done     = (state == WaitSt);
    count    = scount;

    if (reset) {
        state   = S;
        scount  = 0;
        fcount  = 0;
    } else {
        // shift-in for B0–B3
        if (state == B0 || state == B1 ||
            state == B2 || state == B3) {
            scount = (scount << 1) | (data ? 1 : 0);
        }
        // decrement scount once per 1000 fast cycles
        else if (state == CountSt && fcount == 999 && scount > 0) {
            scount = scount - 1;
        }

        // fast counter behavior
        if (state != CountSt) {
            fcount = 0;
        } else if (fcount == 999) {
            fcount = 0;
        } else {
            fcount = fcount + 1;
        }

        // update state
        state = next;
    }
}
