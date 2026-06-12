#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ap_int.h>

// DUT prototype
void TopModule(
    ap_uint<8>  in,
    bool        reset,
    ap_uint<24> &out_bytes,
    bool        &done
);

// Reference model: counter-based (phase 0=idle,1=got_b1,2=got_b2,3=done)
void RefModel(
    ap_uint<8>  in,
    bool        reset,
    ap_uint<24> &out_bytes_ref,
    bool        &done_ref,
    int         &phase,
    ap_uint<24> &buf_ref
) {
    if (reset) {
        phase         = 0;
        buf_ref       = 0;
        done_ref      = false;
        out_bytes_ref = 0;
    } else {
        done_ref      = (phase == 3);
        out_bytes_ref = buf_ref;

        if (phase == 0 || phase == 3) {
            if (in[3] == 1) { buf_ref.range(23, 16) = in; phase = 1; }
            else               phase = 0;
        } else if (phase == 1) {
            buf_ref.range(15, 8) = in; phase = 2;
        } else if (phase == 2) {
            buf_ref.range( 7, 0) = in; phase = 3;
        }
    }
}

int main() {
    const int TOTAL = 500;
    ap_uint<8>  in;
    bool        reset;
    ap_uint<24> out_bytes, out_bytes_ref;
    bool        done, done_ref;
    int         phase   = 0;
    ap_uint<24> buf_ref = 0;
    int         errorCount = 0;

    std::srand(0);
    for (int i = 0; i < TOTAL; ++i) {
        if (i < 2)  reset = true;
        else        reset = (std::rand() & 31) == 0;

        in = std::rand() & 0xFF;
        if ((std::rand() % 10) == 0) in |= 0x08;
        else                          in &= 0xF7;

        TopModule(in, reset, out_bytes, done);
        RefModel(in, reset, out_bytes_ref, done_ref, phase, buf_ref);

        if (done != done_ref) {
            std::cout << "Mismatch at cycle " << i
                      << ": expected done=" << done_ref
                      << ", got=" << done << std::endl;
            errorCount++;
        }
        if (done && out_bytes != out_bytes_ref) {
            std::cout << "Mismatch at cycle " << i
                      << ": expected out_bytes=0x"
                      << std::hex << out_bytes_ref
                      << ", got=0x" << out_bytes
                      << std::dec << std::endl;
            errorCount++;
        }
    }

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
