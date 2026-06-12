#include <iostream>
#include <ap_int.h>

void TopModule(ap_uint<1> in, ap_uint<1> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Apply exhaustive test over all 2 possible input values.
    for (int i = 0; i <= 1; i++) {
        ap_uint<1> in = i;
        ap_uint<1> out;

        TopModule(in, out);

        // Check if the output matches the input.
        if (out != in) {
            std::cout << "Error at cycle " << cycleCount
                      << ": input = " << in
                      << ", expected output = " << in
                      << ", got output = " << out << std::endl;
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
