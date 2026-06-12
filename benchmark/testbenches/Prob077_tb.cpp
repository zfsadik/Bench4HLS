#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool load, ap_uint<10> data, bool &tc);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Golden model: internal counter value.
    ap_uint<10> golden_counter = 0;
    bool expected_tc = false;

    bool load;
    ap_uint<10> data;
    bool dut_tc = false;

    // --- Test Sequence 1: Fixed sequence ---
    // "Count 3, then 10 cycles" sequence.
    std::cout << "Starting fixed sequence: Count 3, then 10 cycles" << std::endl;
    // First, load a count of 3.
    // Cycle 1: load = 1, data = 3.
    load = true;
    data = 3;
    TopModule(load, data, dut_tc);
    // Update golden model.
    golden_counter = 3;
    expected_tc = (golden_counter == 0);
    if (dut_tc != expected_tc) {
        std::cout << "Error at test case " << cycleCount
                  << ": inputs (load,data) = (" << load << ", " << data << ") "
                  << "expected tc = " << expected_tc << ", got tc = " << dut_tc << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Next few cycles: load = 0, count down.
    load = false;
    for (int i = 0; i < 3; i++) {
        TopModule(load, data, dut_tc);
        // Golden model: decrement counter if nonzero.
        if (golden_counter != 0)
            golden_counter = golden_counter - 1;
        expected_tc = (golden_counter == 0);
        if (dut_tc != expected_tc) {
            std::cout << "Error at test case " << cycleCount
                      << ": load=" << load << ", data=" << data
                      << ", expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Now load a count of 10.
    load = true;
    data = 10;
    TopModule(load, data, dut_tc);
    golden_counter = 10;
    expected_tc = (golden_counter == 0);
    if (dut_tc != expected_tc) {
        std::cout << "Error at test case " << cycleCount
                  << ": inputs (load,data) = (" << load << ", " << data << ") "
                  << "expected tc = " << expected_tc << ", got tc = " << dut_tc << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Deassert load and let counter count down for 12 cycles.
    load = false;
    for (int i = 0; i < 12; i++) {
        TopModule(load, data, dut_tc);
        if (golden_counter != 0)
            golden_counter = golden_counter - 1;
        expected_tc = (golden_counter == 0);
        if (dut_tc != expected_tc) {
            std::cout << "Error at test case " << cycleCount
                      << ": load=" << load << ", data=" << data
                      << ", expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Test Sequence 2: Additional fixed tests ---
    // A few more load and count-down tests.
    // Load 0, then load 1023, then let count down.
    load = true; data = 0;
    TopModule(load, data, dut_tc);
    golden_counter = 0; // Load 0.
    expected_tc = (golden_counter == 0);
    if (dut_tc != expected_tc) {
        std::cout << "Error at test case " << cycleCount
                  << ": loaded 0, expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
        errorCount++;
    }
    cycleCount++;

    load = true; data = 1023;
    TopModule(load, data, dut_tc);
    golden_counter = 1023;
    expected_tc = (golden_counter == 0);
    if (dut_tc != expected_tc) {
        std::cout << "Error at test case " << cycleCount
                  << ": loaded 1023, expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
        errorCount++;
    }
    cycleCount++;

    load = false;
    TopModule(load, data, dut_tc);
    if (golden_counter != 0)
        golden_counter = golden_counter - 1;
    expected_tc = (golden_counter == 0);
    if (dut_tc != expected_tc) {
        std::cout << "Error at test case " << cycleCount
                  << ": count down, expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
        errorCount++;
    }
    cycleCount++;

    // --- Test Sequence 3: Random test cases ---
    std::cout << "Starting random test cases" << std::endl;
    for (int i = 0; i < 2500; i++) {
        // Randomly decide load value.
        load = (std::rand() & 0xF) ? false : true; // roughly 1/16 chance to load
        // If load is true, generate a random data value in a reasonable range.
        if (load)
            data = std::rand() % 33; // random number 0 to 32
        // Otherwise, data value is don't care.
        TopModule(load, data, dut_tc);
        // Update golden model.
        if (load) {
            golden_counter = data;
        } else if (golden_counter != 0) {
            golden_counter = golden_counter - 1;
        }
        expected_tc = (golden_counter == 0);
        if (dut_tc != expected_tc) {
            std::cout << "Error at test case " << cycleCount
                      << ": load=" << load << ", data=" << data
                      << ", expected tc=" << expected_tc << ", got tc=" << dut_tc << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
