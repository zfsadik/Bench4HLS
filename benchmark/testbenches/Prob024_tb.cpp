#include <iostream>

void TopModule(bool a, bool b, bool &sum, bool &cout);

int main() {
    int errorCount = 0;

    // Exhaustive truth table for half adder (all 4 input combinations)
    bool test_a[]         = {false, false, true,  true};
    bool test_b[]         = {false, true,  false, true};
    bool expected_sum[]   = {false, true,  true,  false};
    bool expected_cout[]  = {false, false, false, true};
    const int totalCases  = 4;

    for (int i = 0; i < totalCases; i++) {
        bool dut_sum  = false;
        bool dut_cout = false;

        TopModule(test_a[i], test_b[i], dut_sum, dut_cout);

        if (dut_sum != expected_sum[i]) {
            std::cout << "Mismatch in sum at case " << i
                      << ": a = " << test_a[i] << ", b = " << test_b[i]
                      << ", expected sum = " << expected_sum[i]
                      << ", got sum = " << dut_sum << std::endl;
            errorCount++;
        }

        if (dut_cout != expected_cout[i]) {
            std::cout << "Mismatch in cout at case " << i
                      << ": a = " << test_a[i] << ", b = " << test_b[i]
                      << ", expected cout = " << expected_cout[i]
                      << ", got cout = " << dut_cout << std::endl;
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
