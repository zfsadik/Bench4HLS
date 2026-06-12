#include <ap_int.h>

// State encoding as integer literals
enum StateEnum { WL = 0, WR, FALLL, FALLR, DIGL, DIGR, DEAD };

void TopModule(
    bool areset,
    bool bump_left,
    bool bump_right,
    bool ground,
    bool dig,
    bool &walk_left,
    bool &walk_right,
    bool &aaah,
    bool &digging
) {
#pragma HLS INTERFACE ap_none port=areset
#pragma HLS INTERFACE ap_none port=bump_left
#pragma HLS INTERFACE ap_none port=bump_right
#pragma HLS INTERFACE ap_none port=ground
#pragma HLS INTERFACE ap_none port=dig
#pragma HLS INTERFACE ap_none port=walk_left
#pragma HLS INTERFACE ap_none port=walk_right
#pragma HLS INTERFACE ap_none port=aaah
#pragma HLS INTERFACE ap_none port=digging
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    static ap_uint<3> state        = WL;
    static ap_uint<5> fall_counter = 0;
#pragma HLS RESET variable=state
#pragma HLS RESET variable=fall_counter

    if (areset) {
        state        = WL;
        fall_counter = 0;
    }
    else {
        ap_uint<3> next = state;
        switch (state) {
          case WL:
            if (!ground)        next = FALLL;
            else if (dig)       next = DIGL;
            else if (bump_left)  next = WR;
            else                 next = WL;
            break;
          case WR:
            if (!ground)         next = FALLR;
            else if (dig)        next = DIGR;
            else if (bump_right)  next = WL;
            else                  next = WR;
            break;
          case FALLL:
            if (ground) next = (fall_counter >= 20 ? DEAD : WL);
            else        next = FALLL;
            break;
          case FALLR:
            if (ground) next = (fall_counter >= 20 ? DEAD : WR);
            else        next = FALLR;
            break;
          case DIGL:
            next = ground ? DIGL : FALLL;
            break;
          case DIGR:
            next = ground ? DIGR : FALLR;
            break;
          case DEAD:
            next = DEAD;
            break;
          default:
            next = WL;
        }

        if (state == FALLL || state == FALLR) {
            if (fall_counter < 20) fall_counter++;
        } else {
            fall_counter = 0;
        }

        state = next;
    }

    walk_left  = (state == WL);
    walk_right = (state == WR);
    aaah       = (state == FALLL) || (state == FALLR);
    digging    = (state == DIGL)  || (state == DIGR);
}
