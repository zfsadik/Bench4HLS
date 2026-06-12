#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool cin, bool &cout, bool &sum);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Phase 1: Fixed Test Sequence ---
    int fixedTests[] = { 0b000, 0b001, 0b010, 0b011, 0b100, 0b101, 0b110, 0b111 };
    int numFixedTests = sizeof(fixedTests) / sizeof(fixedTests[0]);
    for (int i = 0; i < numFixedTests; i++) {
        int pattern = fixedTests[i];
        bool a   = (pattern >> 2) & 1;  // Extract bit 2.
        bool b   = (pattern >> 1) & 1;  // Extract bit 1.
        bool cin = pattern & 1;         // Extract bit 0.

        bool dut_sum = false;
        bool dut_cout = false;
        TopModule(a, b, cin, dut_cout, dut_sum);

        bool expected_sum = a ^ b ^ cin;
        bool expected_cout = (a & b) | (a & cin) | (b & cin);

        if (dut_sum != expected_sum) {
            std::cout << "Fixed test error (sum) at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b << ", cin = " << cin
                      << ", expected sum = " << expected_sum
                      << ", got = " << dut_sum << std::endl;
            errorCount++;
        }
        if (dut_cout != expected_cout) {
            std::cout << "Fixed test error (cout) at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b << ", cin = " << cin
                      << ", expected cout = " << expected_cout
                      << ", got = " << dut_cout << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 2: Random Test Sequence ---
    const int numRandomTests = 200;
    for (int i = 0; i < numRandomTests; i++) {
        bool a = (std::rand() % 2) ? true : false;
        bool b = (std::rand() % 2) ? true : false;
        bool cin = (std::rand() % 2) ? true : false;

        bool dut_sum = false;
        bool dut_cout = false;
        TopModule(a, b, cin, dut_cout, dut_sum);

        bool expected_sum = a ^ b ^ cin;
        bool expected_cout = (a & b) | (a & cin) | (b & cin);

        if (dut_sum != expected_sum) {
            std::cout << "Random test error (sum) at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b << ", cin = " << cin
                      << ", expected sum = " << expected_sum
                      << ", got = " << dut_sum << std::endl;
            errorCount++;
        }
        if (dut_cout != expected_cout) {
            std::cout << "Random test error (cout) at cycle " << cycleCount
                      << ": a = " << a << ", b = " << b << ", cin = " << cin
                      << ", expected cout = " << expected_cout
                      << ", got = " << dut_cout << std::endl;
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
