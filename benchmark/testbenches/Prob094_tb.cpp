#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<16> a,
               ap_uint<16> b,
               ap_uint<16> c,
               ap_uint<16> d,
               ap_uint<16> e,
               ap_uint<16> f,
               ap_uint<16> g,
               ap_uint<16> h,
               ap_uint<16> i,
               ap_uint<4> sel,
               ap_uint<16> &out);

// Reference function: Computes the expected output.
void ref_compute(ap_uint<16> a,
                 ap_uint<16> b,
                 ap_uint<16> c,
                 ap_uint<16> d,
                 ap_uint<16> e,
                 ap_uint<16> f,
                 ap_uint<16> g,
                 ap_uint<16> h,
                 ap_uint<16> i,
                 ap_uint<4> sel,
                 ap_uint<16> &out) {

    ap_uint<16> inputs[9] = {a, b, c, d, e, f, g, h, i};
    if (sel < 9) out = inputs[sel];
    else out = 0xFFFF;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(0)));

    int cycleCount = 0;
    int errorCount = 0;

    // Define input vectors
    ap_uint<16> a, b, c, d, e, f, g, h, i;
    ap_uint<4> sel;
    ap_uint<16> out_dut;
    ap_uint<16> out_ref;

    // --- Phase 1: Fixed Test Cases ---
    // Fixed initial values:
    a = 0xA;   // Example: hexadecimal A
    b = 0xB;
    c = 0xC;
    d = 0xD;
    e = 0xE;
    f = 0xF;
    g = 0x11;
    h = 0x12;
    i = 0x13;
    // Start with sel = 0.
    sel = 0;
    TopModule(a, b, c, d, e, f, g, h, i, sel, out_dut);
    ref_compute(a, b, c, d, e, f, g, h, i, sel, out_ref);
    if (out_dut != out_ref) {
        std::cout << "Mismatch at cycle " << cycleCount 
                  << ": expected " << out_ref 
                  << ", got " << out_dut << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Apply a series of fixed changes to the selector.
    const int fixedSelCount = 9;
    ap_uint<4> fixedSel[fixedSelCount] = {1, 2, 3, 4, 7, 8, 9, 10, 11};
    for (int idx = 0; idx < fixedSelCount; idx++) {
        sel = fixedSel[idx];
        TopModule(a, b, c, d, e, f, g, h, i, sel, out_dut);
        ref_compute(a, b, c, d, e, f, g, h, i, sel, out_ref);
        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << out_ref 
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Cases ---
    // Apply 200 random test cases.
    for (int i_cycle = 0; i_cycle < 200; i_cycle++) {
        a = static_cast<ap_uint<16>>(std::rand());
        b = static_cast<ap_uint<16>>(std::rand());
        c = static_cast<ap_uint<16>>(std::rand());
        d = static_cast<ap_uint<16>>(std::rand());
        e = static_cast<ap_uint<16>>(std::rand());
        f = static_cast<ap_uint<16>>(std::rand());
        g = static_cast<ap_uint<16>>(std::rand());
        h = static_cast<ap_uint<16>>(std::rand());
        i = static_cast<ap_uint<16>>(std::rand());
        // Generate random 4-bit selector.
        sel = static_cast<ap_uint<4>>(std::rand() % 16);

        TopModule(a, b, c, d, e, f, g, h, i, sel, out_dut);
        ref_compute(a, b, c, d, e, f, g, h, i, sel, out_ref);
        if (out_dut != out_ref) {
            std::cout << "Mismatch at cycle " << cycleCount 
                      << ": expected " << out_ref 
                      << ", got " << out_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    return 0;
}
