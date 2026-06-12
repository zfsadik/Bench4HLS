#include <iostream>
#include <cstdlib>
#include <ctime>

void TopModule(bool a, bool b,
               bool &out_and,
               bool &out_or,
               bool &out_xor,
               bool &out_nand,
               bool &out_nor,
               bool &out_xnor,
               bool &out_anotb);

// Reference function: truth-table lookup indexed by (a<<1)|b.
void ref_compute(bool a, bool b,
                 bool &out_and,
                 bool &out_or,
                 bool &out_xor,
                 bool &out_nand,
                 bool &out_nor,
                 bool &out_xnor,
                 bool &out_anotb)
{
    // index: 0=(0,0) 1=(0,1) 2=(1,0) 3=(1,1)
    const bool tbl_and[4]   = {0,0,0,1};
    const bool tbl_or[4]    = {0,1,1,1};
    const bool tbl_xor[4]   = {0,1,1,0};
    const bool tbl_nand[4]  = {1,1,1,0};
    const bool tbl_nor[4]   = {1,0,0,0};
    const bool tbl_xnor[4]  = {1,0,0,1};
    const bool tbl_anotb[4] = {0,0,1,0};
    int idx = ((int)a << 1) | (int)b;
    out_and   = tbl_and[idx];
    out_or    = tbl_or[idx];
    out_xor   = tbl_xor[idx];
    out_nand  = tbl_nand[idx];
    out_nor   = tbl_nor[idx];
    out_xnor  = tbl_xnor[idx];
    out_anotb = tbl_anotb[idx];
}

int main() {
    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(0)));

    const int fixedTests = 4;  // Test the 4 possible fixed 2-bit combinations.
    const int randomTests = 200; // Number of random test vectors.
    
    int cycleCount = 0;
    int errorCount = 0;
    
    bool a = false;
    bool b = false;
    
    // DUT outputs.
    bool dut_out_and, dut_out_or, dut_out_xor;
    bool dut_out_nand, dut_out_nor, dut_out_xnor, dut_out_anotb;
    // Expected outputs.
    bool exp_out_and, exp_out_or, exp_out_xor;
    bool exp_out_nand, exp_out_nor, exp_out_xnor, exp_out_anotb;
    
    // --- Fixed test vectors ---
    int fixedVectors[4] = {0, 1, 2, 3};
    for (int i = 0; i < fixedTests; i++) {
        int vec = fixedVectors[i];
        a = (vec >> 1) & 0x1;  // Most-significant bit
        b = (vec >> 0) & 0x1;  // Least-significant bit
        
        TopModule(a, b, dut_out_and, dut_out_or, dut_out_xor,
                  dut_out_nand, dut_out_nor, dut_out_xnor, dut_out_anotb);
        
        ref_compute(a, b, exp_out_and, exp_out_or, exp_out_xor,
                    exp_out_nand, exp_out_nor, exp_out_xnor, exp_out_anotb);
        
        // Compare each output and print mismatch if any.
        if (dut_out_and != exp_out_and) {
            std::cout << "Mismatch for out_and at cycle " << cycleCount 
                      << ": expected " << exp_out_and 
                      << ", got " << dut_out_and << std::endl;
            errorCount++;
        }
        if (dut_out_or  != exp_out_or) {
            std::cout << "Mismatch for out_or at cycle " << cycleCount 
                      << ": expected " << exp_out_or 
                      << ", got " << dut_out_or << std::endl;
            errorCount++;
        }
        if (dut_out_xor != exp_out_xor) {
            std::cout << "Mismatch for out_xor at cycle " << cycleCount 
                      << ": expected " << exp_out_xor 
                      << ", got " << dut_out_xor << std::endl;
            errorCount++;
        }
        if (dut_out_nand != exp_out_nand) {
            std::cout << "Mismatch for out_nand at cycle " << cycleCount 
                      << ": expected " << exp_out_nand 
                      << ", got " << dut_out_nand << std::endl;
            errorCount++;
        }
        if (dut_out_nor  != exp_out_nor) {
            std::cout << "Mismatch for out_nor at cycle " << cycleCount 
                      << ": expected " << exp_out_nor 
                      << ", got " << dut_out_nor << std::endl;
            errorCount++;
        }
        if (dut_out_xnor != exp_out_xnor) {
            std::cout << "Mismatch for out_xnor at cycle " << cycleCount 
                      << ": expected " << exp_out_xnor 
                      << ", got " << dut_out_xnor << std::endl;
            errorCount++;
        }
        if (dut_out_anotb != exp_out_anotb) {
            std::cout << "Mismatch for out_anotb at cycle " << cycleCount 
                      << ": expected " << exp_out_anotb 
                      << ", got " << dut_out_anotb << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // --- Random test vectors ---
    for (int i = 0; i < randomTests; i++) {
        int vec = std::rand() % 4; // Random number from 0 to 3.
        a = (vec >> 1) & 0x1;
        b = (vec >> 0) & 0x1;
        
        TopModule(a, b, dut_out_and, dut_out_or, dut_out_xor,
                  dut_out_nand, dut_out_nor, dut_out_xnor, dut_out_anotb);
                  
        ref_compute(a, b, exp_out_and, exp_out_or, exp_out_xor,
                    exp_out_nand, exp_out_nor, exp_out_xnor, exp_out_anotb);
        
        // Compare outputs and print detailed mismatch messages.
        if (dut_out_and != exp_out_and) {
            std::cout << "Mismatch for out_and at cycle " << cycleCount 
                      << ": expected " << exp_out_and 
                      << ", got " << dut_out_and << std::endl;
            errorCount++;
        }
        if (dut_out_or  != exp_out_or) {
            std::cout << "Mismatch for out_or at cycle " << cycleCount 
                      << ": expected " << exp_out_or 
                      << ", got " << dut_out_or << std::endl;
            errorCount++;
        }
        if (dut_out_xor != exp_out_xor) {
            std::cout << "Mismatch for out_xor at cycle " << cycleCount 
                      << ": expected " << exp_out_xor 
                      << ", got " << dut_out_xor << std::endl;
            errorCount++;
        }
        if (dut_out_nand != exp_out_nand) {
            std::cout << "Mismatch for out_nand at cycle " << cycleCount 
                      << ": expected " << exp_out_nand 
                      << ", got " << dut_out_nand << std::endl;
            errorCount++;
        }
        if (dut_out_nor  != exp_out_nor) {
            std::cout << "Mismatch for out_nor at cycle " << cycleCount 
                      << ": expected " << exp_out_nor 
                      << ", got " << dut_out_nor << std::endl;
            errorCount++;
        }
        if (dut_out_xnor != exp_out_xnor) {
            std::cout << "Mismatch for out_xnor at cycle " << cycleCount 
                      << ": expected " << exp_out_xnor 
                      << ", got " << dut_out_xnor << std::endl;
            errorCount++;
        }
        if (dut_out_anotb != exp_out_anotb) {
            std::cout << "Mismatch for out_anotb at cycle " << cycleCount 
                      << ": expected " << exp_out_anotb 
                      << ", got " << dut_out_anotb << std::endl;
            errorCount++;
        }
        cycleCount++;
    }
    
    // Report the test summary exactly as specified.
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount << " mismatches detected out of " 
                  << cycleCount << " cases." << std::endl;
    }
    
    return 0;
}
