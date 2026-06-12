#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<3> sel,
               ap_uint<4> data0,
               ap_uint<4> data1,
               ap_uint<4> data2,
               ap_uint<4> data3,
               ap_uint<4> data4,
               ap_uint<4> data5,
               ap_uint<4> &out);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Initialize random seed.
    std::srand(static_cast<unsigned int>(std::time(0)));

    ap_uint<3> sel;
    ap_uint<4> data0, data1, data2, data3, data4, data5;
    ap_uint<4> dut_out;
    ap_uint<4> expected;

    // Fixed test: Set data inputs to constant values.
    data0 = 0xA;  // 10
    data1 = 0xB;  // 11
    data2 = 0xC;  // 12
    data3 = 0xD;  // 13
    data4 = 0xE;  // 14
    data5 = 0xF;  // 15

    // Start with sel = 7 and cycle through 8 increments.
    sel = 7;
    std::cout << "Starting fixed pattern test: Cycling sel values" << std::endl;
    for (int i = 0; i < 8; i++) {
        TopModule(sel, data0, data1, data2, data3, data4, data5, dut_out);
        // Compute expected output.
        {
            ap_uint<4> inputs[6] = {data0, data1, data2, data3, data4, data5};
            expected = (sel < 6) ? inputs[(int)sel] : ap_uint<4>(0);
        }
        if (dut_out != expected) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (sel, data0, data1, data2, data3, data4, data5) = ("
                      << (unsigned)sel << ", " << (unsigned)data0 << ", " << (unsigned)data1 << ", "
                      << (unsigned)data2 << ", " << (unsigned)data3 << ", " << (unsigned)data4 << ", " << (unsigned)data5
                      << ") expected out = " << (unsigned)expected
                      << ", got out = " << (unsigned)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
        sel = sel + 1;
    }

    // Random test: Apply 100 random test cases.
    std::cout << "Starting random test cases" << std::endl;
    for (int i = 0; i < 100; i++) {
        // Generate random values for sel and the data inputs.
        sel = static_cast<ap_uint<3>>(std::rand() & 0x7);       // 3 bits
        data0 = static_cast<ap_uint<4>>(std::rand() & 0xF);       // 4 bits
        data1 = static_cast<ap_uint<4>>(std::rand() & 0xF);
        data2 = static_cast<ap_uint<4>>(std::rand() & 0xF);
        data3 = static_cast<ap_uint<4>>(std::rand() & 0xF);
        data4 = static_cast<ap_uint<4>>(std::rand() & 0xF);
        data5 = static_cast<ap_uint<4>>(std::rand() & 0xF);

        TopModule(sel, data0, data1, data2, data3, data4, data5, dut_out);
        // Compute expected output.
        {
            ap_uint<4> inputs[6] = {data0, data1, data2, data3, data4, data5};
            expected = (sel < 6) ? inputs[(int)sel] : ap_uint<4>(0);
        }
        if (dut_out != expected) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (sel, data0, data1, data2, data3, data4, data5) = ("
                      << (unsigned)sel << ", " << (unsigned)data0 << ", " << (unsigned)data1 << ", "
                      << (unsigned)data2 << ", " << (unsigned)data3 << ", " << (unsigned)data4 << ", " << (unsigned)data5
                      << ") expected out = " << (unsigned)expected
                      << ", got out = " << (unsigned)dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
