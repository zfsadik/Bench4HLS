#include <iostream>
#include <cstdlib>
#include "ap_int.h"

// DUT
void TopModule(
    bool        areset,
    bool        bump_left,
    bool        bump_right,
    bool        ground,
    bool       &walk_left,
    bool       &walk_right,
    bool       &aaah
);

// Reference model
void ref_model(
    bool        areset,
    bool        bump_left,
    bool        bump_right,
    bool        ground,
    bool       &walk_left,
    bool       &walk_right,
    bool       &aaah,
    ap_uint<2> &state_ref
) {
    const ap_uint<2> WL    = 0;
    const ap_uint<2> WR    = 1;
    const ap_uint<2> FALLL = 2;
    const ap_uint<2> FALLR = 3;

    ap_uint<2> next_state = WL;

    if (state_ref == WL) {
        if (!ground)          next_state = FALLL;
        else if (bump_left)   next_state = WR;
        else                  next_state = WL;
    }
    else if (state_ref == WR) {
        if (!ground)           next_state = FALLR;
        else if (bump_right)   next_state = WL;
        else                   next_state = WR;
    }
    else if (state_ref == FALLL) {
        next_state = ground ? WL : FALLL;
    }
    else { // FALLR
        next_state = ground ? WR : FALLR;
    }

    walk_left  = (state_ref == WL);
    walk_right = (state_ref == WR);
    aaah       = (state_ref == FALLL) || (state_ref == FALLR);

    if (areset) {
        state_ref = WL;
    } else {
        state_ref = next_state;
    }
}

int main() {
    bool areset, bump_left, bump_right, ground;
    bool wl_dut, wr_dut, aaah_dut;
    bool wl_ref, wr_ref, aaah_ref;
    ap_uint<2> state_ref = 0;

    int cycleCount = 0, errorCount = 0;
    std::srand(123);

    // apply reset for 2 cycles
    for (int i = 0; i < 2; i++) {
        TopModule(true, false, false, true, wl_dut, wr_dut, aaah_dut);
        ref_model(true, false, false, true, wl_ref, wr_ref, aaah_ref, state_ref);
    }

    // run 500 cycles
    for (int i = 0; i < 500; i++) {
        areset     = (std::rand() & 63) == 0;
        bump_left  = std::rand() & 1;
        bump_right = std::rand() & 1;
        ground     = ((i / 50) % 2) ? 0 : 1;

        TopModule(areset, bump_left, bump_right, ground,
                  wl_dut, wr_dut, aaah_dut);
        ref_model(areset, bump_left, bump_right, ground,
                  wl_ref, wr_ref, aaah_ref, state_ref);

        if (wl_dut  != wl_ref)   { std::cout<<"Error at test case "<<cycleCount<<": WL expected="<<wl_ref<<", got="<<wl_dut<<"\n"; errorCount++; }
        if (wr_dut  != wr_ref)   { std::cout<<"Error at test case "<<cycleCount<<": WR expected="<<wr_ref<<", got="<<wr_dut<<"\n"; errorCount++; }
        if (aaah_dut!= aaah_ref) { std::cout<<"Error at test case "<<cycleCount<<": AA expected="<<aaah_ref<<", got="<<aaah_dut<<"\n"; errorCount++; }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " cases matched.\n";
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches out of " << cycleCount << " cases.\n";
    }
    return 0;
}
