#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "ap_fixed.h"

// DUT prototype
void TopModule(
    ap_fixed<8,1>       x,
    const ap_fixed<8,1> h[4],
    ap_fixed<28,7>     &y,
    bool               &valid
);

// simple double‐precision reference
double fir_dec2_ref(
    double buf[4],
    const double h[4],
    double       xin,
    int         &toggle,
    bool         vin
) {
    if (vin) {
        buf[3]=buf[2]; buf[2]=buf[1];
        buf[1]=buf[0]; buf[0]=xin;
        double acc=0;
        for(int i=0;i<4;i++) acc += buf[i]*h[i];
        toggle ^= 1;
        if (toggle) return acc;
    }
    return 0.0;
}

int main() {
    std::srand((unsigned)std::time(nullptr));
    const ap_fixed<8,1> h_fix[4] = {0.30011,0.90032,0.90032,0.30011};
    const double        h[4]     = {double(h_fix[0]),double(h_fix[1]),
                                    double(h_fix[2]),double(h_fix[3])};

    double buf0[4]={0}, buf1[4]={0};
    int    tog0=0, tog1=0;
    int    phase_cnt=0;
    bool   f0_produced=false;
    double y0_saved=0.0;
    ap_fixed<28,7> y_dut;
    bool           v_dut;
    int errors=0;
    const int cycles=4000;

    for(int n=0;n<cycles;++n) {
        double xin = (n==0? 0.5 : (std::rand()/double(RAND_MAX))*2.0-1.0);
        ap_fixed<8,1> x_fix = xin;
        double xin_q = double(x_fix);
        TopModule(x_fix, h_fix, y_dut, v_dut);

        bool vin0 = ((phase_cnt & 3)==0);
        bool vin1 = ((phase_cnt & 3)==3) && f0_produced;

        double y0 = fir_dec2_ref(buf0, h, xin_q, tog0, vin0);
        if (vin0 && tog0) {
            ap_fixed<19,4> y0_fix = y0;
            y0_saved = double(y0_fix);
            f0_produced = true;
        }

        double y1 = fir_dec2_ref(buf1, h, y0_saved, tog1, vin1);
        bool   v_ref = vin1 && (bool)tog1;
        if (vin1) f0_produced = false;

        if (v_dut!=v_ref ||
           (v_ref && std::fabs(double(y_dut)-y1)>1e-3)) {
            std::cout<<"Mismatch at cycle "<<n<<"\n";
            ++errors;
        }
        ++phase_cnt;
    }

    if (errors==0)
        std::cout<<"Test Passed: All "<<cycles<<" cases matched.\n";
    else
        std::cout<<"Test Failed: "<<errors<<" mismatches out of "<<cycles<<".\n";

    return 0;
}
