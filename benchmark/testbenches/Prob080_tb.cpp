#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ap_int.h>

void TopModule(ap_uint<1> x, ap_uint<1> y, ap_uint<1> &z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Truth table derived from waveform: XNOR(x,y)
    // x=0,y=0->1  x=1,y=0->0  x=0,y=1->0  x=1,y=1->1
    bool truth[2][2] = {{true, false}, {false, true}};

    // Fixed test sequence.
    // The 2-bit value formed by {y, x} cycles through 0, 1, 2, 3.
    for (int i = 0; i < 4; i++) {
        ap_uint<1> y = (i >> 1) & 1;
        ap_uint<1> x = i & 1;
        ap_uint<1> dut_z;
        TopModule(x, y, dut_z);
        bool expected = truth[x][y];
        if (dut_z != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": x=" << x << ", y=" << y
                      << ", expected z=" << expected
                      << ", got z=" << dut_z << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Random test sequence.
    std::srand(static_cast<unsigned int>(std::time(0)));
    for (int i = 0; i < 100; i++) {
        int rand_val = std::rand() % 4;  // random 2-bit value (0 to 3)
        ap_uint<1> y = (rand_val >> 1) & 1;
        ap_uint<1> x = rand_val & 1;
        ap_uint<1> dut_z;
        TopModule(x, y, dut_z);
        bool expected = truth[x][y];
        if (dut_z != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": x=" << x << ", y=" << y
                      << ", expected z=" << expected
                      << ", got z=" << dut_z << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of "
                  << cycleCount << " cases." << std::endl;
    }

    return 0;
}
