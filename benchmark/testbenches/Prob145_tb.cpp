#include <iostream>
#include <iomanip>
#include <cstdlib>

// DUT prototype
void TopModule(
    bool        reset,
    bool        data,
    bool        done_counting,
    bool        ack,
    bool       &shift_ena,
    bool       &counting,
    bool       &done
);

enum States { S=0, S1, S11, S110, B0, B1, B2, B3, Count, Wait };

// Inline reference model
void RefModel(
    bool        reset,
    bool        data,
    bool        done_counting,
    bool        ack,
    bool       &shift_ena_ref,
    bool       &counting_ref,
    bool       &done_ref,
    States     &state_ref
) {
    States next_ref = S;

    // Next‐state
    switch (state_ref) {
      case S:     next_ref = data ? S1    : S;     break;
      case S1:    next_ref = data ? S11   : S;     break;
      case S11:   next_ref = data ? S11   : S110;  break;
      case S110:  next_ref = data ? B0    : S;     break;
      case B0:    next_ref = B1;                   break;
      case B1:    next_ref = B2;                   break;
      case B2:    next_ref = B3;                   break;
      case B3:    next_ref = Count;                break;
      case Count: next_ref = done_counting ? Wait : Count; break;
      case Wait:  next_ref = ack ? S     : Wait;  break;
      default:    next_ref = S;                   break;
    }

    if (reset) state_ref = S;
    else       state_ref = next_ref;

    // Outputs
    shift_ena_ref = (state_ref == B0) || (state_ref == B1) || (state_ref == B2) || (state_ref == B3);
    counting_ref  = (state_ref == Count);
    done_ref      = (state_ref == Wait);
}

int main() {
    const int TOTAL = 100;
    bool reset, data, done_counting, ack;
    bool shift_ena, counting, done;
    bool shift_ena_ref, counting_ref, done_ref;
    States state_ref = S;
    int errorCount = 0;

    std::srand(0);

    const bool P[4] = {1, 1, 0, 1};

    for (int i = 0; i < TOTAL; ++i) {
        // Reset for first 2 cycles
        if (i < 2) {
            reset = true; data = false; done_counting = false; ack = false;
        }
        // pattern 1,1,0,1 on cycles 2‐5
        else if (i >= 2 && i <= 5) {
            reset = false; data = P[i-2]; done_counting = false; ack = false;
        }
        // shift phase (B0‐B3) + first counting cycle
        else if (i >= 6 && i <= 10) {
            reset = false; data = false; done_counting = false; ack = false;
        }
        // assert done_counting to transition to Wait
        else if (i == 11) {
            reset = false; data = false; done_counting = true; ack = false;
        }
        // hold in Wait without ack
        else if (i >= 12 && i <= 14) {
            reset = false; data = false; done_counting = true; ack = false;
        }
        // ack to return to S
        else if (i == 15) {
            reset = false; data = false; done_counting = true; ack = true;
        }
        // random stimulus
        else {
            reset         = false;
            data          = (std::rand() & 1);
            done_counting = (std::rand() & 7) == 0;
            ack           = (std::rand() & 7) == 0;
        }

        TopModule(reset, data, done_counting, ack, shift_ena, counting, done);
        RefModel(reset, data, done_counting, ack, shift_ena_ref, counting_ref, done_ref, state_ref);

        if (shift_ena != shift_ena_ref) {
            std::cout << "Mismatch at cycle " << i
                      << ": expected shift_ena=" << shift_ena_ref
                      << ", got=" << shift_ena << std::endl;
            errorCount++;
        }
        if (counting != counting_ref) {
            std::cout << "Mismatch at cycle " << i
                      << ": expected counting=" << counting_ref
                      << ", got=" << counting << std::endl;
            errorCount++;
        }
        if (done != done_ref) {
            std::cout << "Mismatch at cycle " << i
                      << ": expected done=" << done_ref
                      << ", got=" << done << std::endl;
            errorCount++;
        }
    }

    // Final summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << TOTAL
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << TOTAL
                  << " cases." << std::endl;
    }
    return 0;
}
