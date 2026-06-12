void TopModule(bool reset, bool bump_left, bool bump_right,
               bool & walk_left, bool & walk_right) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=bump_left
    #pragma HLS INTERFACE ap_none port=bump_right
    #pragma HLS INTERFACE ap_none port=walk_left
    #pragma HLS INTERFACE ap_none port=walk_right
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool state = false;       // initial state: WL
    #pragma HLS RESET variable=state

    bool cur_state = state;

    if (reset) {
        state = false; // Reset to Walk Left
    } else {
        if (!cur_state) { // Currently walking left
            state = (bump_left ? true : false);
        } else {      // Currently walking right
            state = (bump_right ? false : true);
        }
    }

    walk_left = !cur_state;
    walk_right = cur_state;
}
