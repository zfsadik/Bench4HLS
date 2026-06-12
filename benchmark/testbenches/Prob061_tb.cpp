#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(ap_uint<5> a, ap_uint<5> b, ap_uint<5> c, 
               ap_uint<5> d, ap_uint<5> e, ap_uint<5> f,
               ap_uint<8> &w, ap_uint<8> &x, ap_uint<8> &y, ap_uint<8> &z);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    const int numFixedTests = 4;   // We'll run 4 fixed test cases
    const int numRandomTests = 100;  // And 100 random test cases

    // --- Fixed Test Sequence ---
    struct TestCase {
        ap_uint<5> a, b, c, d, e, f;
    };
    // Predefined test cases (values given in decimal)
    TestCase fixedTests[] = {
        { 1, 2, 3, 4, 5, 6 },
        { 31, 0, 15, 16, 8, 7 },
        { 10, 10, 10, 10, 10, 10 },
        { 0,  1,  2,  3,  4,  5 }
    };

    for (int i = 0; i < numFixedTests; i++) {
        ap_uint<5> a = fixedTests[i].a;
        ap_uint<5> b = fixedTests[i].b;
        ap_uint<5> c = fixedTests[i].c;
        ap_uint<5> d = fixedTests[i].d;
        ap_uint<5> e = fixedTests[i].e;
        ap_uint<5> f = fixedTests[i].f;

        // Concatenation order: {a, b, c, d, e, f, 2'b11}
        ap_uint<32> concat = ((((( (ap_uint<32>)a << 5) | b) << 5 | c) << 5 | d) << 5 | e) << 5 | f;
        concat = (concat << 2) | 3;
        // Expected outputs.
        ap_uint<8> expected_w = concat.range(31,24);
        ap_uint<8> expected_x = concat.range(23,16);
        ap_uint<8> expected_y = concat.range(15,8);
        ap_uint<8> expected_z = concat.range(7,0);

        // DUT outputs.
        ap_uint<8> w_dut, x_dut, y_dut, z_dut;
        TopModule(a, b, c, d, e, f, w_dut, x_dut, y_dut, z_dut);

        // Hardcoded cross-check for test[0]: a=1,b=2,c=3,d=4,e=5,f=6 -> w=0x08,x=0x86,y=0x42,z=0x9B
        if (i == 0) {
            if (w_dut != 0x08 || x_dut != 0x86 || y_dut != 0x42 || z_dut != 0x9B) {
                std::cout << "Hardcoded check failed: expected w=0x08 x=0x86 y=0x42 z=0x9B"
                          << ", got w=0x" << std::hex << (unsigned int)w_dut
                          << " x=0x" << (unsigned int)x_dut
                          << " y=0x" << (unsigned int)y_dut
                          << " z=0x" << (unsigned int)z_dut << std::dec << std::endl;
                errorCount++;
            }
        }

        if (w_dut != expected_w) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected w=" << (unsigned int)expected_w 
                      << ", got w=" << (unsigned int)w_dut << std::endl;
            errorCount++;
        }
        if (x_dut != expected_x) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected x=" << (unsigned int)expected_x 
                      << ", got x=" << (unsigned int)x_dut << std::endl;
            errorCount++;
        }
        if (y_dut != expected_y) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected y=" << (unsigned int)expected_y 
                      << ", got y=" << (unsigned int)y_dut << std::endl;
            errorCount++;
        }
        if (z_dut != expected_z) {
            std::cout << "Fixed test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected z=" << (unsigned int)expected_z 
                      << ", got z=" << (unsigned int)z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Random Test Sequence ---
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < numRandomTests; i++) {
        ap_uint<5> a = std::rand() % 32;
        ap_uint<5> b = std::rand() % 32;
        ap_uint<5> c = std::rand() % 32;
        ap_uint<5> d = std::rand() % 32;
        ap_uint<5> e = std::rand() % 32;
        ap_uint<5> f = std::rand() % 32;

        // DUT outputs.
        ap_uint<8> w_dut, x_dut, y_dut, z_dut;
        TopModule(a, b, c, d, e, f, w_dut, x_dut, y_dut, z_dut);

        // Compute expected outputs.
        ap_uint<32> concat = ((((( (ap_uint<32>)a << 5) | b) << 5 | c) << 5 | d) << 5 | e) << 5 | f;
        concat = (concat << 2) | 3;
        ap_uint<8> expected_w = concat.range(31,24);
        ap_uint<8> expected_x = concat.range(23,16);
        ap_uint<8> expected_y = concat.range(15,8);
        ap_uint<8> expected_z = concat.range(7,0);

        if (w_dut != expected_w) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected w=" << (unsigned int)expected_w 
                      << ", got w=" << (unsigned int)w_dut << std::endl;
            errorCount++;
        }
        if (x_dut != expected_x) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected x=" << (unsigned int)expected_x 
                      << ", got x=" << (unsigned int)x_dut << std::endl;
            errorCount++;
        }
        if (y_dut != expected_y) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected y=" << (unsigned int)expected_y 
                      << ", got y=" << (unsigned int)y_dut << std::endl;
            errorCount++;
        }
        if (z_dut != expected_z) {
            std::cout << "Random test error at cycle " << cycleCount
                      << ": a=" << (unsigned int)a 
                      << ", b=" << (unsigned int)b 
                      << ", c=" << (unsigned int)c 
                      << ", d=" << (unsigned int)d 
                      << ", e=" << (unsigned int)e 
                      << ", f=" << (unsigned int)f 
                      << " => expected z=" << (unsigned int)expected_z 
                      << ", got z=" << (unsigned int)z_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount 
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
