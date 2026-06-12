#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

void TopModule(bool mode, bool too_cold, bool too_hot, bool fan_on, bool &heater, bool &aircon, bool &fan);

void extract_inputs(int vec, bool &too_cold, bool &too_hot, bool &mode, bool &fan_on) {
    too_cold = (vec >> 3) & 0x1;
    too_hot  = (vec >> 2) & 0x1;
    mode     = (vec >> 1) & 0x1;
    fan_on   = (vec >> 0) & 0x1;
}

void compute_expected(bool mode, bool too_cold, bool too_hot, bool fan_on,
                      bool &heater_exp, bool &aircon_exp, bool &fan_exp) {
    heater_exp = mode && too_cold;
    aircon_exp = (!mode) && too_hot;
    fan_exp    = heater_exp || aircon_exp || fan_on;
}

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool too_cold, too_hot, mode, fan_on;
    bool heater_dut, aircon_dut, fan_dut;
    bool heater_exp, aircon_exp, fan_exp;

    // -----------------------
    // Winter Test Vectors
    // (mode = 1, heating)
    // Test vectors (in hex, representing 4-bit {too_cold, too_hot, mode, fan_on}):
    //   0x2 (0010), 0x2 (0010), 0xA (1010), 0xB (1011),
    //   0x2 (0010), 0x3 (0011), 0x2 (0010), 0x6 (0110),
    //   0xE (1110), 0x7 (0111), 0xF (1111)
    int winter_tests[] = {0x2, 0x2, 0xA, 0xB, 0x2, 0x3, 0x2, 0x6, 0xE, 0x7, 0xF};
    int num_winter = sizeof(winter_tests) / sizeof(winter_tests[0]);

    for (int i = 0; i < num_winter; i++) {
        int vec = winter_tests[i];
        extract_inputs(vec, too_cold, too_hot, mode, fan_on);
        compute_expected(mode, too_cold, too_hot, fan_on, heater_exp, aircon_exp, fan_exp);
        TopModule(mode, too_cold, too_hot, fan_on, heater_dut, aircon_dut, fan_dut);

        if (heater_dut != heater_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected heater = " << (int)heater_exp 
                      << ", got heater = " << (int)heater_dut << std::endl;
            errorCount++;
        }
        if (aircon_dut != aircon_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected aircon = " << (int)aircon_exp 
                      << ", got aircon = " << (int)aircon_dut << std::endl;
            errorCount++;
        }
        if (fan_dut != fan_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected fan = " << (int)fan_exp 
                      << ", got fan = " << (int)fan_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // -----------------------
    // Summer Test Vectors
    // (mode = 0, cooling)
    // Test vectors:
    //   0x0 (0000), 0x0 (0000), 0x4 (0100), 0x5 (0101),
    //   0x0 (0000), 0x1 (0001), 0x0 (0000), 0x8 (1000),
    //   0xC (1100), 0x9 (1001), 0xD (1101)
    int summer_tests[] = {0x0, 0x0, 0x4, 0x5, 0x0, 0x1, 0x0, 0x8, 0xC, 0x9, 0xD};
    int num_summer = sizeof(summer_tests) / sizeof(summer_tests[0]);

    for (int i = 0; i < num_summer; i++) {
        int vec = summer_tests[i];
        extract_inputs(vec, too_cold, too_hot, mode, fan_on);
        compute_expected(mode, too_cold, too_hot, fan_on, heater_exp, aircon_exp, fan_exp);
        TopModule(mode, too_cold, too_hot, fan_on, heater_dut, aircon_dut, fan_dut);

        if (heater_dut != heater_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected heater = " << (int)heater_exp 
                      << ", got heater = " << (int)heater_dut << std::endl;
            errorCount++;
        }
        if (aircon_dut != aircon_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected aircon = " << (int)aircon_exp 
                      << ", got aircon = " << (int)aircon_dut << std::endl;
            errorCount++;
        }
        if (fan_dut != fan_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected fan = " << (int)fan_exp 
                      << ", got fan = " << (int)fan_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Random Test Vectors
    // Apply 200 random test cases.
    std::srand(0); // Fixed seed for reproducibility.
    for (int i = 0; i < 200; i++) {
        int vec = std::rand() % 16; // Random 4-bit value.
        extract_inputs(vec, too_cold, too_hot, mode, fan_on);
        compute_expected(mode, too_cold, too_hot, fan_on, heater_exp, aircon_exp, fan_exp);
        TopModule(mode, too_cold, too_hot, fan_on, heater_dut, aircon_dut, fan_dut);

        if (heater_dut != heater_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected heater = " << (int)heater_exp 
                      << ", got heater = " << (int)heater_dut << std::endl;
            errorCount++;
        }
        if (aircon_dut != aircon_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected aircon = " << (int)aircon_exp 
                      << ", got aircon = " << (int)aircon_dut << std::endl;
            errorCount++;
        }
        if (fan_dut != fan_exp) {
            std::cout << "Error at test case " << cycleCount 
                      << ": inputs (too_cold, too_hot, mode, fan_on) = ("
                      << (int)too_cold << ", " << (int)too_hot << ", " << (int)mode << ", " << (int)fan_on
                      << ") expected fan = " << (int)fan_exp 
                      << ", got fan = " << (int)fan_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Final Test Summary Report
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
