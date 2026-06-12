#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool in, bool &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 120 cycles with random inputs.
    for (int i = 0; i < 120; i++) {
        bool in = (std::rand() % 2);
        bool out;
        TopModule(in, out);
        bool expected = in;
        if (out != expected) {
            std::cout << "Error at cycle " << cycleCount
                      << ": input = " << in
                      << ", expected = " << expected
                      << ", got = " << out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
