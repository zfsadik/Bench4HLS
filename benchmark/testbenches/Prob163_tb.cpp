
#include <iostream>
#include <cstdlib>
#include "ap_int.h"
#define WIDTH 8

void TopModule(
    ap_uint<WIDTH> a,
    ap_uint<WIDTH> b,
    ap_uint<WIDTH> m,
    ap_uint<WIDTH> mprime,
    ap_uint<WIDTH> &result
);

static ap_uint<WIDTH> modinv(ap_uint<WIDTH> m) {
    int64_t t = 0, newt = 1;
    int64_t r = (int64_t)1 << WIDTH, newr = m.to_uint();
    while (newr) {
        int64_t q    = r / newr;
        int64_t tmpt = newt;
        newt         = t - q * newt;
        t            = tmpt;
        int64_t tmpr = newr;
        newr         = r - q * newr;
        r            = tmpr;
    }
    if (t < 0) t += ((int64_t)1 << WIDTH);
    return ap_uint<WIDTH>(t);
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int tc = 0; tc < 16; ++tc) {
        ap_uint<WIDTH> a = ap_uint<WIDTH>(std::rand() & ((1 << WIDTH) - 1));
        ap_uint<WIDTH> b = ap_uint<WIDTH>(std::rand() & ((1 << WIDTH) - 1));
        ap_uint<WIDTH> m;
        do {
            m = ap_uint<WIDTH>(std::rand() & ((1 << WIDTH) - 1));
        } while ((m & 1) == 0 || m.to_uint() <= 1);

        ap_uint<WIDTH> mprime = ap_uint<WIDTH>(-modinv(m));

        ap_uint<WIDTH> dut_res;
        TopModule(a, b, m, mprime, dut_res);

        uint32_t exp = 1;
        uint32_t base = a.to_uint() % m.to_uint();
        for (int i = WIDTH - 1; i >= 0; --i) {
            exp = (uint64_t(exp) * exp) % m.to_uint();
            if (b[i]) exp = (uint64_t(exp) * base) % m.to_uint();
        }
        ap_uint<WIDTH> expected = ap_uint<WIDTH>(exp);

        if (dut_res != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (a,b,m) = ("
                      << a.to_uint() << ", " << b.to_uint() << ", " << m.to_uint() << ")"
                      << " expected res = " << expected.to_uint()
                      << ", got res = "   << dut_res.to_uint() << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
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
