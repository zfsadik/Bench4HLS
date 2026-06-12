#include <iostream>
#include <cstdlib>

// DUT prototype
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
);

enum RefStates { WL=0, WR, FALLL, FALLR, DIGL, DIGR };

// Reference model
void RefModel(
    bool         reset,
    bool         bump_left,
    bool         bump_right,
    bool         ground,
    bool         dig,
    RefStates   &state_ref,
    bool        &walk_left_ref,
    bool        &walk_right_ref,
    bool        &aaah_ref,
    bool        &digging_ref
) {
    if (reset) {
        state_ref = WL;
    } else {
        RefStates next_ref = state_ref;
        switch (state_ref) {
          case WL:
            if (!ground)        next_ref = FALLL;
            else if (dig)       next_ref = DIGL;
            else if (bump_left) next_ref = WR;
            else                next_ref = WL;
            break;
          case WR:
            if (!ground)         next_ref = FALLR;
            else if (dig)        next_ref = DIGR;
            else if (bump_right) next_ref = WL;
            else                 next_ref = WR;
            break;
          case FALLL:
            next_ref = ground ? WL : FALLL;
            break;
          case FALLR:
            next_ref = ground ? WR : FALLR;
            break;
          case DIGL:
            next_ref = ground ? DIGL : FALLL;
            break;
          case DIGR:
            next_ref = ground ? DIGR : FALLR;
            break;
        }
        state_ref = next_ref;
    }
    walk_left_ref  = (state_ref == WL);
    walk_right_ref = (state_ref == WR);
    aaah_ref       = (state_ref == FALLL) || (state_ref == FALLR);
    digging_ref    = (state_ref == DIGL)  || (state_ref == DIGR);
}

int main() {
    const int TOTAL = 1000;
    bool reset, bump_left, bump_right, ground, dig;
    bool walk_left, walk_right, aaah, digging;
    bool walk_left_ref, walk_right_ref, aaah_ref, digging_ref;
    RefStates state_ref = WL;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    // Reset stimulus: >=2 cycles
    for (int i = 0; i < 2; i++) {
        TopModule(true, false, false, true, false,
                  walk_left, walk_right, aaah, digging);
        RefModel(true, false, false, true, false, state_ref,
                 walk_left_ref, walk_right_ref, aaah_ref, digging_ref);
    }

    // Normal stimulus
    for (int i = 0; i < TOTAL; i++) {
        reset      = false;
        bump_left  = (std::rand() & 1);
        bump_right = (std::rand() & 1);
        ground     = (std::rand() & 3) != 0;
        dig        = (std::rand() & 7) == 0;

        TopModule(reset, bump_left, bump_right, ground, dig,
                  walk_left, walk_right, aaah, digging);
        RefModel(reset, bump_left, bump_right, ground, dig, state_ref,
                 walk_left_ref, walk_right_ref, aaah_ref, digging_ref);

        if (walk_left != walk_left_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected walk_left=" << walk_left_ref
                      << ", got=" << walk_left << std::endl;
            errorCount++;
        }
        if (walk_right != walk_right_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected walk_right=" << walk_right_ref
                      << ", got=" << walk_right << std::endl;
            errorCount++;
        }
        if (aaah != aaah_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected aaah=" << aaah_ref
                      << ", got=" << aaah << std::endl;
            errorCount++;
        }
        if (digging != digging_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected digging=" << digging_ref
                      << ", got=" << digging << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }
    return 0;
}
