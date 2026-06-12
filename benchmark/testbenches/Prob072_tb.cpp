#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool areset, bool train_valid, bool train_taken, ap_uint<2> &state);

int main() {
    int errorCount = 0;
    int cycleCount = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned int>(std::time(0)));

    ap_uint<2> goldenState = 1;
    ap_uint<2> dut_state;

    // Function to simulate one clock cycle.
    auto simulateCycle = [&](bool areset, bool train_valid, bool train_taken) {
        TopModule(areset, train_valid, train_taken, dut_state);
    };

    // Function to update the golden (reference) state.
    auto updateGolden = [&](bool areset, bool train_valid, bool train_taken) {
        if (areset)
            goldenState = 1;
        else if (train_valid) {
            if (train_taken && goldenState < 3)
                goldenState = goldenState + 1;
            else if (!train_taken && goldenState > 0)
                goldenState = goldenState - 1;
        }
    };

    // Test Sequence 1: Synchronous reset test
    for (int i = 0; i < 5; i++) {
        bool areset = true;
        bool train_valid = false;
        bool train_taken = false;
        simulateCycle(areset, train_valid, train_taken);
        updateGolden(areset, train_valid, train_taken);
        if (dut_state != goldenState) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (areset, train_valid, train_taken) = ("
                      << areset << ", " << train_valid << ", " << train_taken
                      << ") expected state = " << (unsigned)goldenState
                      << ", got state = " << (unsigned)dut_state << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Release reset for one cycle.
    {
        bool areset = false;
        bool train_valid = false;
        bool train_taken = false;
        simulateCycle(areset, train_valid, train_taken);
        updateGolden(areset, train_valid, train_taken);
        if (dut_state != goldenState) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (areset, train_valid, train_taken) = ("
                      << areset << ", " << train_valid << ", " << train_taken
                      << ") expected state = " << (unsigned)goldenState
                      << ", got state = " << (unsigned)dut_state << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Test Sequence 2: Count up then down
    // Count up: drive train_valid = true, train_taken = true.
    for (int i = 0; i < 5; i++) {
        bool areset = false;
        bool train_valid = true;
        bool train_taken = true;
        simulateCycle(areset, train_valid, train_taken);
        updateGolden(areset, train_valid, train_taken);
        if (dut_state != goldenState) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (areset, train_valid, train_taken) = ("
                      << areset << ", " << train_valid << ", " << train_taken
                      << ") expected state = " << (unsigned)goldenState
                      << ", got state = " << (unsigned)dut_state << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    // Count down: drive train_valid = true, train_taken = false.
    for (int i = 0; i < 5; i++) {
        bool areset = false;
        bool train_valid = true;
        bool train_taken = false;
        simulateCycle(areset, train_valid, train_taken);
        updateGolden(areset, train_valid, train_taken);
        if (dut_state != goldenState) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (areset, train_valid, train_taken) = ("
                      << areset << ", " << train_valid << ", " << train_taken
                      << ") expected state = " << (unsigned)goldenState
                      << ", got state = " << (unsigned)dut_state << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Test Sequence 3: Random test cases
    // Simulate 1000 random cycles.
    for (int i = 0; i < 1000; i++) {
        bool areset = (std::rand() % 2) ? true : false;
        bool train_valid = (std::rand() % 2) ? true : false;
        bool train_taken = (std::rand() % 2) ? true : false;
        simulateCycle(areset, train_valid, train_taken);
        updateGolden(areset, train_valid, train_taken);
        if (dut_state != goldenState) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (areset, train_valid, train_taken) = ("
                      << areset << ", " << train_valid << ", " << train_taken
                      << ") expected state = " << (unsigned)goldenState
                      << ", got state = " << (unsigned)dut_state << std::endl;
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
