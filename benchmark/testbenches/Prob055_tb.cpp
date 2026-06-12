#include <iostream>

void TopModule(bool a, bool b, bool c, bool d, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int fixedCases = 16;   // All 16 possible combinations for 4 input bits.

    // Truth table derived directly from K-map in prompt.
    // Index = (a<<3)|(b<<2)|(c<<1)|d
    const bool truth_table[16] = {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1};

    // --- Fixed Test Sequence ---
    for (int i = 0; i < fixedCases; i++) {
        // Extract each bit.
        bool a = (i >> 3) & 0x1;
        bool b = (i >> 2) & 0x1;
        bool c = (i >> 1) & 0x1;
        bool d = i & 0x1;

        bool dut_out;
        TopModule(a, b, c, d, dut_out);

        bool expected = truth_table[i];

        if (dut_out != expected) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a=" << a << ", b=" << b << ", c=" << c << ", d=" << d
                      << ", expected out=" << expected
                      << ", got out=" << dut_out << std::endl;
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
