void TopModule(bool mode, bool too_cold, bool too_hot, bool fan_on, bool &heater, bool &aircon, bool &fan) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_none port=mode
#pragma HLS INTERFACE ap_none port=too_cold
#pragma HLS INTERFACE ap_none port=too_hot
#pragma HLS INTERFACE ap_none port=fan_on
#pragma HLS INTERFACE ap_none port=heater
#pragma HLS INTERFACE ap_none port=aircon
#pragma HLS INTERFACE ap_none port=fan

    heater = mode && too_cold;
    aircon = (!mode) && too_hot;
    fan    = heater || aircon || fan_on;
}