#include <iostream>
#include <cstdlib>
#include "ap_int.h"

// DUT
void TopModule(bool a, bool &p, bool &q);

// Reference model
void ref_model(bool a, bool &p, bool &q, bool &prev_a) {
    p = a;
    q = prev_a;
    prev_a = a;
}

int main() {
    bool a     = false;
    bool p_dut, q_dut;
    bool p_ref, q_ref;
    bool prev_a = false;
    int  errorCount = 0, cycleCount = 0;

    std::srand(42);
    for (int i = 0; i < 1000; ++i) {
        a = std::rand() & 1;

        TopModule(a, p_dut, q_dut);
        ref_model(a, p_ref, q_ref, prev_a);

        if (p_dut != p_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": p expected " << p_ref
                      << ", got "       << p_dut << std::endl;
            errorCount++;
        }
        if (q_dut != q_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": q expected " << q_ref
                      << ", got "       << q_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }
    return 0;
}
