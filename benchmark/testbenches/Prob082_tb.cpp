#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool reset, bool load, bool ena, ap_uint<4> data, ap_uint<4> &q);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    std::srand(static_cast<unsigned int>(std::time(0)));

    // Golden model state
    ap_uint<4> golden = 0;
    // DUT output
    ap_uint<4> dut_q = 0;

    bool reset, load, ena;
    ap_uint<4> data;

    std::cout << "Starting fixed test sequence..." << std::endl;

    // ---- Cycles 0-1: assert reset (2 cycles) ----
    reset = true; load = false; ena = false; data = 0;
    for (int i = 0; i < 2; i++) {
        TopModule(reset, load, ena, data, dut_q);
        golden = 0;
        if (dut_q != golden) {
            std::cout << "Error at test case " << cycleCount
                      << ": reset asserted, expected q = " << (unsigned)golden
                      << ", got q = " << (unsigned)dut_q << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // ---- Cycle 2: load = 1, data = 0xA ----
    reset = false; load = true; ena = false; data = 0xA;
    TopModule(reset, load, ena, data, dut_q);
    golden = data;
    if (dut_q != golden) {
        std::cout << "Error at test case " << cycleCount
                  << ": load asserted, expected q = " << (unsigned)golden
                  << ", got q = " << (unsigned)dut_q << std::endl;
        errorCount++;
    }
    cycleCount++;

    // ---- Cycle 3: shift right ----
    reset = false; load = false; ena = true;
    TopModule(reset, load, ena, data, dut_q);
    golden = golden >> 1;
    if (dut_q != golden) {
        std::cout << "Error at test case " << cycleCount
                  << ": shift operation, expected q = " << (unsigned)golden
                  << ", got q = " << (unsigned)dut_q << std::endl;
        errorCount++;
    }
    cycleCount++;

    // ---- Cycle 4: no operation ----
    reset = false; load = false; ena = false;
    TopModule(reset, load, ena, data, dut_q);
    if (dut_q != golden) {
        std::cout << "Error at test case " << cycleCount
                  << ": no operation, expected q = " << (unsigned)golden
                  << ", got q = " << (unsigned)dut_q << std::endl;
        errorCount++;
    }
    cycleCount++;

    // ---- Random test sequence ----
    std::cout << "Starting random test sequence..." << std::endl;
    for (int i = 0; i < 400; i++) {
        // Random stimuli
        reset = (std::rand() & 1);
        load   = (std::rand() & 1);
        ena    = (std::rand() & 1);
        data   = static_cast<ap_uint<4>>(std::rand() & 0xF);

        TopModule(reset, load, ena, data, dut_q);

        // Update golden model just like RTL
        ap_uint<4> expected = golden;
        if (reset) {
            expected = 0;
        } else if (load) {
            expected = data;
        } else if (ena) {
            expected = expected >> 1;
        }
        // else expected stays the same

        if (dut_q != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": random test, reset=" << reset
                      << ", load=" << load
                      << ", ena=" << ena
                      << ", data=0x" << std::hex << (unsigned)data << std::dec
                      << ", expected q=0x" << std::hex << (unsigned)expected << std::dec
                      << ", got q=0x"   << std::hex << (unsigned)dut_q    << std::dec
                      << std::endl;
            errorCount++;
        }

        // Commit golden for next iteration
        golden = expected;
        cycleCount++;
    }

    // ---- Summary ----
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
