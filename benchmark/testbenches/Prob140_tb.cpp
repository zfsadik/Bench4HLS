#include <iostream>
#include <cstdlib>
#include <ap_int.h>

// DUT prototype
void TopModule(ap_uint<1> in, ap_uint<1> reset, ap_uint<8> &out_byte, ap_uint<1> &done);

// Reference model — independent counter-based behavioral implementation
void RefModule(ap_uint<1> in, ap_uint<1> reset, ap_uint<8> &out_byte_ref, ap_uint<1> &done_ref) {
    enum Phase { IDLE, RECV, ERR_WAIT };
    static Phase      phase    = IDLE;
    static int        bit_cnt  = 0;
    static ap_uint<8> byte_buf = 0;

    done_ref     = 0;
    out_byte_ref = 0;

    if (reset) {
        phase    = IDLE;
        bit_cnt  = 0;
        byte_buf = 0;
        return;
    }

    switch (phase) {
      case IDLE:
        if (!in) { phase = RECV; bit_cnt = 0; byte_buf = 0; }
        break;
      case RECV:
        if (bit_cnt < 8) {
            byte_buf[bit_cnt] = in;
            bit_cnt++;
        } else {
            if (in) {
                done_ref     = 1;
                out_byte_ref = byte_buf;
                phase        = IDLE;
            } else {
                phase = ERR_WAIT;
            }
        }
        break;
      case ERR_WAIT:
        if (in) phase = IDLE;
        break;
    }
}

int main() {
    const int TOTAL_CYCLES = 2500;
    ap_uint<1> in = 1, reset = 1;
    ap_uint<8> out_byte, out_byte_ref;
    ap_uint<1> done = 0, done_ref = 0;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    // Drive reset for 2 cycles before normal stimulus
    for (int i = 0; i < 2; i++) {
        in = 1; reset = 1;
        TopModule(in, reset, out_byte, done);
        RefModule(in, reset, out_byte_ref, done_ref);
        cycleCount++;
    }

    // Main stimulus: random serial data and occasional resets
    for (int i = 0; i < TOTAL_CYCLES - 2; i++) {
        reset = ((std::rand() % 50) == 0) ? 1 : 0;
        in    = ((std::rand() & 1) != 0)  ? 1 : 0;

        TopModule(in, reset, out_byte, done);
        RefModule(in, reset, out_byte_ref, done_ref);

        if (done != done_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected done=" << done_ref
                      << ", got done="       << done << std::endl;
            errorCount++;
        }
        if (done && out_byte != out_byte_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected out_byte=" << out_byte_ref
                      << ", got "       << out_byte << std::endl;
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
