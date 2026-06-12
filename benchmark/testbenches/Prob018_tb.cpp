#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<256> in, ap_uint<8> sel, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int totalCycles = 1000; // Run 1000 test cycles

    // Deterministic corner cases
    struct { ap_uint<256> in; ap_uint<8> sel; } corners[] = {
        { ap_uint<256>(0), ap_uint<8>(0) },
        { ap_uint<256>(0), ap_uint<8>(255) },
        { ~ap_uint<256>(0), ap_uint<8>(0) },
        { ~ap_uint<256>(0), ap_uint<8>(255) },
        { ap_uint<256>(1), ap_uint<8>(0) },
        { ap_uint<256>(1), ap_uint<8>(1) },
    };
    for (auto &c : corners) {
        bool dutOut = false;
        TopModule(c.in, c.sel, dutOut);
        bool expected = c.in[c.sel];
        if (dutOut != expected) {
            std::cout << "Corner mismatch at cycle " << cycleCount
                      << ": sel = " << c.sel
                      << ", expected = " << expected
                      << ", got = " << dutOut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int i = 0; i < totalCycles; i++) {
        // Generate a random 256-bit input by combining 8 chunks of 32 bits.
        ap_uint<256> in = 0;
        for (int j = 0; j < 8; j++) {
            ap_uint<32> chunk = static_cast<ap_uint<32>>(std::rand());
            in |= ((ap_uint<256>)chunk) << (32 * j);
        }

        // Generate a random 8-bit selector (0 to 255).
        ap_uint<8> sel = static_cast<ap_uint<8>>(std::rand() % 256);

        bool dutOut = false;
        TopModule(in, sel, dutOut);

        bool expected = in[sel];

        if (dutOut != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": sel = " << sel
                      << ", expected = " << expected
                      << ", got = " << dutOut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
