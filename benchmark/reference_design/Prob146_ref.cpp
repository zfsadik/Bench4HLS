void TopModule(
    bool    reset,
    bool    bump_left,
    bool    bump_right,
    bool    ground,
    bool    dig,
    bool   &walk_left,
    bool   &walk_right,
    bool   &aaah,
    bool   &digging
) {
    #pragma HLS INTERFACE ap_none port=reset
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

    enum States { WL=0, WR, FALLL, FALLR, DIGL, DIGR };
    static States state = WL;
    #pragma HLS RESET variable=state
    States next = state;

    if (reset) {
        state = WL;
    } else {
        // Combinational next-state
        switch (state) {
          case WL:
            if (!ground)       next = FALLL;
            else if (dig)      next = DIGL;
            else if (bump_left)next = WR;
            else               next = WL;
            break;
          case WR:
            if (!ground)        next = FALLR;
            else if (dig)       next = DIGR;
            else if (bump_right)next = WL;
            else                next = WR;
            break;
          case FALLL:
            next = ground ? WL : FALLL;
            break;
          case FALLR:
            next = ground ? WR : FALLR;
            break;
          case DIGL:
            next = ground ? DIGL : FALLL;
            break;
          case DIGR:
            next = ground ? DIGR : FALLR;
            break;
        }
        state = next;
    }

    // Output logic (Moore)
    walk_left  = (state == WL);
    walk_right = (state == WR);
    aaah       = (state == FALLL) || (state == FALLR);
    digging    = (state == DIGL ) || (state == DIGR );
}
