#include <iostream>
#include "ap_int.h"

void TopModule(ap_uint<4> x, ap_uint<4> y, ap_uint<5> &sum);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    for (int xi = 0; xi < 16; xi++) {
        for (int yi = 0; yi < 16; yi++) {
            ap_uint<4> x = xi;
            ap_uint<4> y = yi;
            ap_uint<5> dutSum;

            TopModule(x, y, dutSum);

            ap_uint<5> expectedSum = x + y;

            // Compare the DUT output with the expected result.
            if (dutSum != expectedSum) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": x = " << x << ", y = " << y
                          << ", expected sum = " << expectedSum
                          << ", got sum = " << dutSum << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
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
