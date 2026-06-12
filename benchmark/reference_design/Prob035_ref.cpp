void TopModule(bool ring, bool vibrate_mode, bool &ringer, bool &motor) {

    #pragma HLS INTERFACE ap_none port=ring
    #pragma HLS INTERFACE ap_none port=vibrate_mode
    #pragma HLS INTERFACE ap_none port=ringer
    #pragma HLS INTERFACE ap_none port=motor
    #pragma HLS INTERFACE ap_ctrl_none port=return

    ringer = ring && (!vibrate_mode);
    motor = ring && vibrate_mode;
}
