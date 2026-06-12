#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ap_int.h>

void TopModule(
    bool reset,
    bool ena,
    bool &pm,
    ap_uint<8> &hh,
    ap_uint<8> &mm,
    ap_uint<8> &ss
);

// Arithmetic reference model: independent of BCD update logic in TopModule.
// Maintains an absolute second counter (0..43199) and a PM flag.
// Derives BCD outputs via integer division — no BCD carry logic to mirror.
void RefModule(
    bool reset,
    bool ena,
    bool &pm_ref,
    ap_uint<8> &hh_ref,
    ap_uint<8> &mm_ref,
    ap_uint<8> &ss_ref
) {
    static int  r_sec = 0;   // 0..43199  (43200 seconds per 12-hour half)
    static bool r_pm  = 0;

    if (reset) {
        r_sec = 0;
        r_pm  = 0;
    } else if (ena) {
        if (r_sec == 43199) { r_pm = !r_pm; r_sec = 0; }
        else                { r_sec++; }
    }

    int s  = r_sec % 60;
    int m  = (r_sec / 60) % 60;
    int h  = r_sec / 3600;            // 0..11
    int hd = (h == 0) ? 12 : h;      // 12,1,2,...,11

    pm_ref  = r_pm;
    hh_ref  = ((hd / 10) << 4) | (hd % 10);
    mm_ref  = ((m  / 10) << 4) | (m  % 10);
    ss_ref  = ((s  / 10) << 4) | (s  % 10);
}

int main() {
    const int TOTAL = 5000;
    bool reset, ena;
    bool pm_dut, pm_ref;
    ap_uint<8> hh_dut, mm_dut, ss_dut;
    ap_uint<8> hh_ref, mm_ref, ss_ref;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    for (int i = 0; i < 2; i++) {
        TopModule(true, false, pm_dut, hh_dut, mm_dut, ss_dut);
        RefModule(true, false, pm_ref, hh_ref, mm_ref, ss_ref);
    }

    for (int i = 0; i < TOTAL; ++i) {
        reset = ((i % 300) == 0);
        ena   = ((std::rand() & 1) != 0);

        TopModule(reset, ena, pm_dut, hh_dut, mm_dut, ss_dut);
        RefModule(reset, ena, pm_ref, hh_ref, mm_ref, ss_ref);

        if (pm_dut != pm_ref ||
            hh_dut != hh_ref ||
            mm_dut != mm_ref ||
            ss_dut != ss_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected "
                      << (pm_ref ? "PM " : "AM ")
                      << std::hex << std::setw(2) << std::setfill('0') << hh_ref << ":"
                      << std::setw(2) << mm_ref << ":"
                      << std::setw(2) << ss_ref
                      << ", got "
                      << (pm_dut ? "PM " : "AM ")
                      << std::setw(2) << hh_dut << ":"
                      << std::setw(2) << mm_dut << ":"
                      << std::setw(2) << ss_dut
                      << std::dec << std::setfill(' ')
                      << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Summary
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
