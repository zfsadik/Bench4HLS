#include <iostream>

void TopModule(bool in1, bool in2, bool in3, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int i = 0; i < 8; i++) {
        bool in1 = (i >> 2) & 1;
        bool in2 = (i >> 1) & 1;
        bool in3 = (i >> 0) & 1;

        bool dutOut = false;
        TopModule(in1, in2, in3, dutOut);

        bool expected = (in1 == in2) ^ in3;

        if (dutOut != expected) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": in1 = " << in1 << ", in2 = " << in2 << ", in3 = " << in3
                      << ", expected = " << expected << ", got = " << dutOut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
