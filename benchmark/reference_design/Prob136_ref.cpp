#include <ap_int.h>

void TopModule(
    bool reset,
    bool ena,
    bool &pm,
    ap_uint<8> &hh,
    ap_uint<8> &mm,
    ap_uint<8> &ss
) {
    #pragma HLS INTERFACE ap_none port=reset
    #pragma HLS INTERFACE ap_none port=ena
    #pragma HLS INTERFACE ap_none port=pm
    #pragma HLS INTERFACE ap_none port=hh
    #pragma HLS INTERFACE ap_none port=mm
    #pragma HLS INTERFACE ap_none port=ss
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool        pm_reg    = 0;
    #pragma HLS RESET variable=pm_reg
    static ap_uint<8>  hh_reg    = 0x12;  // 12:00:00 AM initial
    #pragma HLS RESET variable=hh_reg
    static ap_uint<8>  mm_reg    = 0x00;
    #pragma HLS RESET variable=mm_reg
    static ap_uint<8>  ss_reg    = 0x00;
    #pragma HLS RESET variable=ss_reg

    if (reset) {
        // Reset to 12:00:00 AM
        pm_reg = 0;
        hh_reg = 0x12;
        mm_reg = 0x00;
        ss_reg = 0x00;
    }
    else if (ena) {
        // Compute roll‑over conditions
        bool su_roll = ((ss_reg & 0x0F) == 9);
        bool st_roll = (ss_reg == 0x59);
        bool mu_roll = (mm_reg.range(3,0) == 9) && st_roll;
        bool mt_roll = (mm_reg == 0x59) && st_roll;
        bool hu_roll = (hh_reg.range(3,0) == 9) && mt_roll;
        bool pm_toggle = (hh_reg == 0x11) && (mm_reg == 0x59) && (ss_reg == 0x59);

        // Seconds units
        {
            ap_uint<4> su = ss_reg.range(3,0);
            if (su_roll) ss_reg.range(3,0) = 0;
            else          ss_reg.range(3,0) = su + 1;
        }
        // Seconds tens
        if (su_roll) {
            ap_uint<4> st = ss_reg.range(7,4);
            if (st == 5) ss_reg.range(7,4) = 0;
            else         ss_reg.range(7,4) = st + 1;
        }
        // Minutes units
        if (st_roll) {
            ap_uint<4> mu = mm_reg.range(3,0);
            if (mu == 9) mm_reg.range(3,0) = 0;
            else         mm_reg.range(3,0) = mu + 1;
        }
        // Minutes tens
        if (mu_roll) {
            ap_uint<4> mt = mm_reg.range(7,4);
            if (mt == 5) mm_reg.range(7,4) = 0;
            else         mm_reg.range(7,4) = mt + 1;
        }
        // Hours units
        if (mt_roll) {
            if (hh_reg == 0x12) {
                hh_reg = 0x01;
            } else {
                ap_uint<4> hu = hh_reg.range(3,0);
                if (hu == 9) hh_reg.range(3,0) = 0;
                else         hh_reg.range(3,0) = hu + 1;
            }
        }
        // Hours tens and 12→1
        if (hu_roll) {
            ap_uint<4> ht = hh_reg.range(7,4);
            hh_reg.range(7,4) = ht + 1;
        }
        // Toggle PM at 11:59:59 → 12:00:00
        if (pm_toggle) {
            pm_reg = !pm_reg;
        }
    }

    // Drive outputs
    pm = pm_reg;
    hh = hh_reg;
    mm = mm_reg;
    ss = ss_reg;
}
