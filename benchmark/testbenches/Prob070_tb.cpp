#include <ap_int.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// DUT prototype
void TopModule(
    bool          resetn,
    ap_uint<2>    byteena,
    ap_uint<16>   d,
    ap_uint<16>& q
);

// reference model
void referenceUpdate(bool resetn, ap_uint<2> byteena, ap_uint<16> d, ap_uint<16> &q_ref, ap_uint<16> &r) {
    if (!resetn) {
        r = 0;
    } else {
        if (byteena[0]) r.range(7,0)  = d.range(7,0);
        if (byteena[1]) r.range(15,8) = d.range(15,8);
    }
    q_ref = r;
}

int main() {
    srand(time(NULL));

    bool           resetn;
    ap_uint<2>     byteena;
    ap_uint<16>    d;
    ap_uint<16>    q_dut, q_ref;
    ap_uint<16>    r          = 0;
    unsigned long  cycleCount = 0, errorCount = 0;

    auto tick = [&]() {
        TopModule(resetn, byteena, d, q_dut);
        referenceUpdate(resetn, byteena, d, q_ref, r);

        if (q_dut != q_ref) {
            cout << "Mismatch at cycle " << cycleCount
                 << ": expected 0x" << hex << (unsigned)q_ref
                 << ", got 0x" << (unsigned)q_dut << dec << endl;
            errorCount++;
        }
        cycleCount++;
    };

    // 1) Reset behavior — 2 cycles
    resetn   = false;
    byteena  = 0b11;
    d        = 0xABCD;
    tick();
    tick();
    resetn = true;
    tick(); tick();    // two cycles post‑reset

    // 2) Byte‑enable exercise
    byteena = 0b11;
    d       = rand() & 0xFFFF;
    tick();
    for (int i = 0; i < 10; i++) {
        d       = rand() & 0xFFFF;
        byteena = byteena + 1;
        tick();
    }

    // 2b) Hold-value test — byteena=0b00 should not update register
    byteena = 0b00;
    d       = 0xFFFF;
    tick(); tick();

    // 3) Long random sequence
    for (int i = 0; i < 400; i++) {
        byteena[0] = (rand() & 3) != 0;
        byteena[1] = (rand() & 3) != 0;
        d           = rand() & 0xFFFF;
        tick();
    }

    // final summary
    if (errorCount == 0) {
        cout << "Test Passed: All " << cycleCount
             << " test cases matched." << endl;
    } else {
        cout << "Test Failed: " << errorCount
             << " mismatches detected out of "
             << cycleCount << " cases." << endl;
    }
    return 0;
}
