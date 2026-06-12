#include <iostream>

void TopModule(bool a, bool b, bool c, bool &out);

int main() {
    int errorCount = 0;

    // Exhaustive truth table derived from the K-map (all 8 minterms).
    struct TestCase { bool a, b, c, expected; };
    TestCase cases[] = {
        {false, false, false, false},
        {false, false, true,  true },
        {false, true,  false, true },
        {false, true,  true,  true },
        {true,  false, false, true },
        {true,  false, true,  true },
        {true,  true,  false, true },
        {true,  true,  true,  true },
    };
    const int totalCases = 8;

    for (int i = 0; i < totalCases; i++) {
        bool dut_out = false;
        TopModule(cases[i].a, cases[i].b, cases[i].c, dut_out);

        if (dut_out != cases[i].expected) {
            std::cout << "Mismatch at minterm " << i
                      << ": a=" << cases[i].a
                      << " b=" << cases[i].b
                      << " c=" << cases[i].c
                      << " expected=" << cases[i].expected
                      << " got=" << dut_out << std::endl;
            errorCount++;
        }
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << totalCases << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << totalCases << " cases." << std::endl;
    }

    return 0;
}
