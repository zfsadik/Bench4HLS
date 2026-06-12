#include <iostream>
#include <cstdlib>
#include <ap_int.h>

// DUT prototype
void TopModule(
    bool        d,
    bool        done_counting,
    bool        ack,
    ap_uint<10> state,
    bool       &B3_next,
    bool       &S_next,
    bool       &S1_next,
    bool       &Count_next,
    bool       &Wait_next,
    bool       &done,
    bool       &counting,
    bool       &shift_ena
);

// Reference model — behavioral (state-machine walk, independent of one-hot equations)
void RefModel(
    bool        d,
    bool        done_counting,
    bool        ack,
    ap_uint<10> state,
    bool       &B3_ref,
    bool       &S_ref,
    bool       &S1_ref,
    bool       &Count_ref,
    bool       &Wait_ref,
    bool       &done_ref,
    bool       &counting_ref,
    bool       &shift_ena_ref
) {
    // Decode one-hot to integer
    int cur = -1;
    for (int i = 0; i < 10; i++) if (state[i]) { cur = i; break; }

    B3_ref = S_ref = S1_ref = Count_ref = Wait_ref = false;
    done_ref = counting_ref = shift_ena_ref = false;

    // Next-state: 0=S 1=S1 2=S11 3=S110 4=B0 5=B1 6=B2 7=B3 8=Count 9=Wait
    int nxt = cur;
    switch (cur) {
        case 0: nxt = d ? 1 : 0; break;
        case 1: nxt = d ? 2 : 0; break;
        case 2: nxt = d ? 2 : 3; break;
        case 3: nxt = d ? 4 : 0; break;
        case 4: nxt = 5; break;
        case 5: nxt = 6; break;
        case 6: nxt = 7; break;
        case 7: nxt = 8; break;
        case 8: nxt = done_counting ? 9 : 8; break;
        case 9: nxt = ack ? 0 : 9; break;
        default: break;
    }

    if (nxt == 7) B3_ref    = true;
    if (nxt == 0) S_ref     = true;
    if (nxt == 1) S1_ref    = true;
    if (nxt == 8) Count_ref = true;
    if (nxt == 9) Wait_ref  = true;

    done_ref      = (cur == 9);
    counting_ref  = (cur == 8);
    shift_ena_ref = (cur >= 4 && cur <= 7);
}

int main() {
    const int TOTAL = 1000;
    int cycleCount = 0, errorCount = 0;
    std::srand(0);

    for (int i = 0; i < TOTAL; ++i) {
        // Random inputs
        bool d             = (std::rand() & 1);
        bool done_counting = (std::rand() & 1);
        bool ack           = (std::rand() & 1);
        int  idx           = std::rand() % 10;
        ap_uint<10> state  = (ap_uint<10>)1 << idx;

        // DUT outputs
        bool B3_dut, S_dut, S1_dut, Count_dut, Wait_dut;
        bool done_dut, counting_dut, shift_ena_dut;
        TopModule(d, done_counting, ack, state,
                  B3_dut, S_dut, S1_dut, Count_dut, Wait_dut,
                  done_dut, counting_dut, shift_ena_dut);

        // Reference outputs
        bool B3_ref, S_ref, S1_ref, Count_ref, Wait_ref;
        bool done_ref, counting_ref, shift_ena_ref;
        RefModel(d, done_counting, ack, state,
                 B3_ref, S_ref, S1_ref, Count_ref, Wait_ref,
                 done_ref, counting_ref, shift_ena_ref);

        // Compare and report
        if (B3_dut != B3_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected B3_next=" << B3_ref
                      << ", got=" << B3_dut << "\n";
            errorCount++;
        }
        if (S_dut != S_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected S_next=" << S_ref
                      << ", got=" << S_dut << "\n";
            errorCount++;
        }
        if (S1_dut != S1_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected S1_next=" << S1_ref
                      << ", got=" << S1_dut << "\n";
            errorCount++;
        }
        if (Count_dut != Count_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected Count_next=" << Count_ref
                      << ", got=" << Count_dut << "\n";
            errorCount++;
        }
        if (Wait_dut != Wait_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected Wait_next=" << Wait_ref
                      << ", got=" << Wait_dut << "\n";
            errorCount++;
        }
        if (done_dut != done_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected done=" << done_ref
                      << ", got=" << done_dut << "\n";
            errorCount++;
        }
        if (counting_dut != counting_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected counting=" << counting_ref
                      << ", got=" << counting_dut << "\n";
            errorCount++;
        }
        if (shift_ena_dut != shift_ena_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected shift_ena=" << shift_ena_ref
                      << ", got=" << shift_ena_dut << "\n";
            errorCount++;
        }
        cycleCount++;
    }

    // Final summary
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
