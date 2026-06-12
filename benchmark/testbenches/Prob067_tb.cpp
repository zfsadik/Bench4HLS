#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b, bool c, bool d, bool &out_sop, bool &out_pos);

int main() {
    int errorCount = 0;
    int cycleCount = 0;
    bool a, b, c, d;
    bool out_sop_dut, out_pos_dut;

    // Truth table indexed by 4-bit val (a=MSB, d=LSB).
    // Minterms (output=1): 2, 7, 15
    // Maxterms (output=0): 0, 1, 4, 5, 6, 9, 10, 13, 14
    // Don't cares (skip check): 3, 8, 11, 12  => -1
    // Both out_sop and out_pos must equal the defined output on non-don't-care inputs.
    int truth[16] = {0, 0, 1, -1, 0, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0, 1};

    // 1. Predefined test vectors.
    int predefined[] = {0, 1, 2, 4, 5, 6, 7, 9, 10, 13, 14, 15};
    int num_predefined = sizeof(predefined) / sizeof(predefined[0]);

    for (int i = 0; i < num_predefined; i++) {
        int val = predefined[i];
        // Decompose the 4-bit value into inputs (a is the MSB).
        a = (val >> 3) & 0x1;
        b = (val >> 2) & 0x1;
        c = (val >> 1) & 0x1;
        d = (val >> 0) & 0x1;

        // Call the DUT.
        TopModule(a, b, c, d, out_sop_dut, out_pos_dut);

        if (truth[val] != -1) {
            bool exp = (bool)truth[val];
            if (out_sop_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_sop = " << (int)exp
                          << ", got out_sop = " << (int)out_sop_dut << std::endl;
                errorCount++;
            }
            if (out_pos_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_pos = " << (int)exp
                          << ", got out_pos = " << (int)out_pos_dut << std::endl;
                errorCount++;
            }
        }
        cycleCount++;
    }

    // 2. Test all 16 possible input combinations.
    for (int val = 0; val < 16; val++) {
        a = (val >> 3) & 0x1;
        b = (val >> 2) & 0x1;
        c = (val >> 1) & 0x1;
        d = (val >> 0) & 0x1;

        TopModule(a, b, c, d, out_sop_dut, out_pos_dut);

        if (truth[val] != -1) {
            bool exp = (bool)truth[val];
            if (out_sop_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_sop = " << (int)exp
                          << ", got out_sop = " << (int)out_sop_dut << std::endl;
                errorCount++;
            }
            if (out_pos_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_pos = " << (int)exp
                          << ", got out_pos = " << (int)out_pos_dut << std::endl;
                errorCount++;
            }
        }
        cycleCount++;
    }

    // 3. Apply 50 random test vectors.
    std::srand(0); // Fixed seed for reproducibility.
    for (int i = 0; i < 50; i++) {
        int val = std::rand() % 16;
        a = (val >> 3) & 0x1;
        b = (val >> 2) & 0x1;
        c = (val >> 1) & 0x1;
        d = (val >> 0) & 0x1;

        TopModule(a, b, c, d, out_sop_dut, out_pos_dut);

        if (truth[val] != -1) {
            bool exp = (bool)truth[val];
            if (out_sop_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_sop = " << (int)exp
                          << ", got out_sop = " << (int)out_sop_dut << std::endl;
                errorCount++;
            }
            if (out_pos_dut != exp) {
                std::cout << "Error at test case " << cycleCount
                          << ": inputs (a, b, c, d) = (" << (int)a << ", " << (int)b
                          << ", " << (int)c << ", " << (int)d
                          << ") expected out_pos = " << (int)exp
                          << ", got out_pos = " << (int)out_pos_dut << std::endl;
                errorCount++;
            }
        }
        cycleCount++;
    }

    // Final test summary report.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount << " cases." << std::endl;
    }

    return 0;
}
