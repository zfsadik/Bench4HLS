void TopModule(
    bool        reset,
    bool        data,
    bool        done_counting,
    bool        ack,
    bool       &shift_ena,
    bool       &counting,
    bool       &done
) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=data
    #pragma HLS INTERFACE ap_none port=done_counting
    #pragma HLS INTERFACE ap_none port=ack
    #pragma HLS INTERFACE ap_none port=shift_ena
    #pragma HLS INTERFACE ap_none port=counting
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // One‐hot state encoding:
    enum States { S=0, S1, S11, S110, B0, B1, B2, B3, Count, Wait };
    static States state = S;
    #pragma HLS RESET variable=state
    States next = S;

    // Combinational next‐state
    switch (state) {
      case S:     next = data ? S1    : S;     break;
      case S1:    next = data ? S11   : S;     break;
      case S11:   next = data ? S11   : S110;  break;
      case S110:  next = data ? B0    : S;     break;
      case B0:    next = B1;                     break;
      case B1:    next = B2;                     break;
      case B2:    next = B3;                     break;
      case B3:    next = Count;                  break;
      case Count: next = done_counting ? Wait : Count; break;
      case Wait:  next = ack ? S     : Wait;    break;
      default:    next = S;                     break;
    }

    // Sequential state update
    if (reset) state = S;
    else       state = next;

    // Output logic
    shift_ena = (state == B0) || (state == B1) || (state == B2) || (state == B3);
    counting  = (state == Count);
    done      = (state == Wait);
}
