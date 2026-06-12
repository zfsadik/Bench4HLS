#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<1> a, ap_uint<1> b, ap_uint<1> c, ap_uint<1> d, ap_uint<1> &out, ap_uint<1> &out_n);

// Pre-computed truth table: index = (d<<3)|(c<<2)|(b<<1)|a, out = (a&b)|(c&d)
static const ap_uint<1> expected_out_table[16] = {
    0, 0, 0, 1,
    0, 0, 0, 1,
    0, 0, 0, 1,
    1, 1, 1, 1
};

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Exhaustive test: Test all 16 combinations for inputs a, b, c, and d.
    for (int val = 0; val < 16; val++) {
        ap_uint<1> a = (val >> 0) & 1;
        ap_uint<1> b = (val >> 1) & 1;
        ap_uint<1> c = (val >> 2) & 1;
        ap_uint<1> d = (val >> 3) & 1;

        ap_uint<1> dut_out, dut_out_n;
        TopModule(a, b, c, d, dut_out, dut_out_n);

        ap_uint<1> expected_out = expected_out_table[val];
        ap_uint<1> expected_out_n = ~expected_out;

        if (dut_out != expected_out) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (a, b, c, d) = (" << a << ", " << b << ", " << c << ", " << d << ") "
                      << "expected out = " << expected_out
                      << ", got out = " << dut_out << std::endl;
            errorCount++;
        }
        if (dut_out_n != expected_out_n) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (a, b, c, d) = (" << a << ", " << b << ", " << c << ", " << d << ") "
                      << "expected out_n = " << expected_out_n
                      << ", got out_n = " << dut_out_n << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Random test: Execute 100 random test cases.
    std::srand(static_cast<unsigned>(std::time(0)));
    for (int i = 0; i < 100; i++) {
        int rnd = std::rand() & 0xF;
        ap_uint<1> a = (rnd >> 0) & 1;
        ap_uint<1> b = (rnd >> 1) & 1;
        ap_uint<1> c = (rnd >> 2) & 1;
        ap_uint<1> d = (rnd >> 3) & 1;

        ap_uint<1> dut_out, dut_out_n;
        TopModule(a, b, c, d, dut_out, dut_out_n);

        ap_uint<1> expected_out = expected_out_table[rnd];
        ap_uint<1> expected_out_n = ~expected_out;

        if (dut_out != expected_out) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (a, b, c, d) = (" << a << ", " << b << ", " << c << ", " << d << ") "
                      << "expected out = " << expected_out
                      << ", got out = " << dut_out << std::endl;
            errorCount++;
        }
        if (dut_out_n != expected_out_n) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (a, b, c, d) = (" << a << ", " << b << ", " << c << ", " << d << ") "
                      << "expected out_n = " << expected_out_n
                      << ", got out_n = " << dut_out_n << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
