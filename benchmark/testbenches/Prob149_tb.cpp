#include <iostream>
#include <cstdlib>
#include <ap_int.h>

enum StateEnum { WL = 0, WR, FALLL, FALLR, DIGL, DIGR, DEAD };

// DUT declaration
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
);

int main() {
    bool areset      = true;
    bool bump_left   = false;
    bool bump_right  = false;
    bool ground      = true;
    bool dig         = false;
    bool walk_left, walk_right, aaah, digging;

    // Reference model state
    ap_uint<3> state_ref        = WL;
    ap_uint<5> fall_counter_ref = 0;
    ap_uint<3> next_ref         = WL;
    bool walk_left_ref, walk_right_ref, aaah_ref, digging_ref;

    unsigned errorCount = 0, cycleCount = 0;
    std::srand(42);

    // ** Initial synchronous reset (>=2 cycles) **
    for (int i = 0; i < 2; i++) {
        TopModule(areset, bump_left, bump_right, ground, dig,
                  walk_left, walk_right, aaah, digging);
        state_ref        = WL;
        fall_counter_ref = 0;
    }
    // De-assert reset
    areset = false;

    struct Stim { bool bl, br, g, dg; int cycles; };
    Stim tests[] = {
      {0,0,1,0,5},    // walk
      {1,0,1,0,3},    // bump_left
      {0,1,1,0,3},    // bump_right
      {0,0,1,1,5},    // dig
      {0,0,0,0,20}, {0,0,1,0,3},  // fall-survive
      {0,0,0,0,21}, {0,0,1,0,3},  // fall-splat
      {0,0,1,0,0}    // long random
    };

    for (auto &t : tests) {
        int runs = t.cycles > 0 ? t.cycles : 1000;
        for (int i = 0; i < runs; ++i) {
            // apply inputs (random if t.cycles==0)
            if (t.cycles > 0) {
                bump_left  = t.bl;
                bump_right = t.br;
                ground     = t.g;
                dig        = t.dg;
            } else {
                bump_left  = std::rand() & 1;
                bump_right = std::rand() & 1;
                ground     = std::rand() & 1;
                dig        = std::rand() & 1;
            }

            // ** one call = one rising edge **
            TopModule(areset, bump_left, bump_right, ground, dig,
                      walk_left, walk_right, aaah, digging);

            // Reference model
            if (!areset) {
                switch (state_ref) {
                  case WL:
                    if (!ground)       next_ref = FALLL;
                    else if (dig)      next_ref = DIGL;
                    else if (bump_left) next_ref = WR;
                    else                next_ref = WL;
                    break;
                  case WR:
                    if (!ground)        next_ref = FALLR;
                    else if (dig)       next_ref = DIGR;
                    else if (bump_right) next_ref = WL;
                    else                 next_ref = WR;
                    break;
                  case FALLL:
                    next_ref = ground
                               ? (fall_counter_ref >= 20 ? DEAD : WL)
                               : FALLL;
                    break;
                  case FALLR:
                    next_ref = ground
                               ? (fall_counter_ref >= 20 ? DEAD : WR)
                               : FALLR;
                    break;
                  case DIGL:
                    next_ref = ground ? DIGL : FALLL;
                    break;
                  case DIGR:
                    next_ref = ground ? DIGR : FALLR;
                    break;
                  case DEAD:
                    next_ref = DEAD;
                    break;
                  default:
                    next_ref = WL;
                }

                // update fall counter
                if (state_ref == FALLL || state_ref == FALLR) {
                    if (fall_counter_ref < 20) fall_counter_ref++;
                } else {
                    fall_counter_ref = 0;
                }
                state_ref = next_ref;
            } else {
                // reset in reference
                state_ref        = WL;
                fall_counter_ref = 0;
            }

            // Reference outputs
            walk_left_ref  = (state_ref == WL);
            walk_right_ref = (state_ref == WR);
            aaah_ref       = (state_ref == FALLL) || (state_ref == FALLR);
            digging_ref    = (state_ref == DIGL)  || (state_ref == DIGR);

            // Self‑check all four signals
            if (walk_left  != walk_left_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected walk_left=" << walk_left_ref
                          << ", got " << walk_left << "\n";
                errorCount++;
            }
            if (walk_right != walk_right_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected walk_right=" << walk_right_ref
                          << ", got " << walk_right << "\n";
                errorCount++;
            }
            if (aaah       != aaah_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected aaah=" << aaah_ref
                          << ", got " << aaah << "\n";
                errorCount++;
            }
            if (digging    != digging_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected digging=" << digging_ref
                          << ", got " << digging << "\n";
                errorCount++;
            }

            ++cycleCount;
        }
    }

    // Final summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches in " << cycleCount << " cycles." << std::endl;
    }
    return 0;
}
