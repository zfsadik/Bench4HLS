#include <iostream>
#include <vector>
#include <cstdlib>
#include <ap_int.h>

// DUT prototype
void TopModule(
    bool reset,
    bool data,
    ap_uint<4> &count,
    bool &counting,
    bool &done,
    bool ack
);

// Reference FSM states
enum RefState { S=0, S1, S11, S110, B0, B1, B2, B3, CountSt, WaitSt };

int main() {
    bool reset         = true;
    bool data          = false;
    bool ack           = false;
    ap_uint<4> count_dut, count_ref;
    bool counting_dut = false, counting_ref = false;
    bool done_dut     = false, done_ref     = false;

    // Reference registers
    RefState stateR   = S;
    RefState nextR    = S;
    ap_uint<4> scR    = 0;
    ap_uint<10> fcR   = 0;

    unsigned errorCount = 0, cycleCount = 0;

    // helper: run one clk cycle and compare
    auto cycle = [&](bool rst, bool dat, bool ack_in) {
        TopModule(rst, dat, count_dut, counting_dut, done_dut, ack_in);

        // reference next-state
        bool done_countR = (stateR == CountSt) && (fcR == 999) && (scR == 0);
        switch (stateR) {
          case S:       nextR = dat     ? S1     : S;      break;
          case S1:      nextR = dat     ? S11    : S;      break;
          case S11:     nextR = dat     ? S11    : S110;   break;
          case S110:    nextR = dat     ? B0     : S;      break;
          case B0:      nextR = B1;                         break;
          case B1:      nextR = B2;                         break;
          case B2:      nextR = B3;                         break;
          case B3:      nextR = CountSt;                    break;
          case CountSt: nextR = done_countR ? WaitSt : CountSt; break;
          case WaitSt:  nextR = ack_in  ? S      : WaitSt; break;
          default:      nextR = S;                          break;
        }

        // reference outputs from current state (before update)
        counting_ref = (stateR == CountSt);
        done_ref     = (stateR == WaitSt);
        count_ref    = scR;

        // shift / decrement
        if (stateR == B0 || stateR == B1 ||
            stateR == B2 || stateR == B3) {
            scR = (scR << 1) | (dat ? 1 : 0);
        } else if (stateR == CountSt && fcR == 999 && scR > 0) {
            scR = scR - 1;
        }

        // fast counter
        if (stateR != CountSt) {
            fcR = 0;
        } else if (fcR == 999) {
            fcR = 0;
        } else {
            fcR = fcR + 1;
        }

        // sync reset + state update
        if (rst) {
            stateR = S;
            scR     = 0;
            fcR     = 0;
        } else {
            stateR = nextR;
        }

        // compare
        if (count_dut != count_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected count=" << count_ref
                      << ", got "        << count_dut << "\n";
            errorCount++;
        }
        if (counting_dut != counting_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected counting=" << counting_ref
                      << ", got "                << counting_dut << "\n";
            errorCount++;
        }
        if (done_dut != done_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected done=" << done_ref
                      << ", got "          << done_dut << "\n";
            errorCount++;
        }

        cycleCount++;
    };

    // Test 1: delay=1 → 2000 cycles
    cycle(true,  false, false);  // reset pulse
    reset = false;
    for (bool b : {1,1,0,1}) cycle(false,b,false);      // "1101"
    for (bool b : {0,0,0,1}) cycle(false,b,false);      // delay=1
    unsigned cnt = 0;
    for (int i=0; i<2000; i++) {
        cycle(false,false,false);
        if (counting_dut) cnt++;
    }
    if (cnt!=2000)
        std::cout<<"Hint: first test counted "<<cnt<<" cycles\n";
    cycle(false,false,false); cycle(false,false,true); cycle(false,false,false);

    // Test 2: delay=14 → 15000 cycles
    for (bool b : {1,1,0,1}) cycle(false,b,false);
    for (bool b : {1,1,1,0}) cycle(false,b,false);      // delay=14
    cnt = 0;
    for (int i=0; i<15000; i++) {
        cycle(false,false,false);
        if (counting_dut) cnt++;
    }
    if (cnt!=15000)
        std::cout<<"Hint: second test counted "<<cnt<<" cycles\n";
    cycle(false,false,false); cycle(false,false,true); cycle(false,false,false);

    // Random stress
    for (int i=0; i<100000; i++) {
        bool d = rand() & 1;
        bool a = rand() & 1;
        bool r = (rand() & 8191) == 0;
        cycle(r, d, a);
    }

    // Summary
    if (errorCount==0) {
        std::cout<<"Test Passed: All "<<cycleCount<<" cycles matched.\n";
    } else {
        std::cout<<"Test Failed: "<<errorCount
                 <<" mismatches out of "<<cycleCount<<" cycles.\n";
    }
    return 0;
}
