#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

// Declaration of the TopModule function.
void TopModule(ap_uint<32> in, ap_uint<32>& out);

int main() {
    const int test_cycles = 110;  // Total number of test iterations.
    int errors = 0;

    // Seed the random number generator.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Run an initial 10 cycles with random values followed by 100 additional random cycles.
    for (int cycle = 0; cycle < test_cycles; cycle++) {
        // Generate a random 32-bit input.
        ap_uint<32> in_val = ((ap_uint<32>)std::rand() << 16) ^ std::rand();
        
        // Compute the expected result by reversing the byte order:
        // Extract each byte and reposition them appropriately.
        ap_uint<32> expected = ((in_val & 0x000000FF) << 24) |
                               ((in_val & 0x0000FF00) << 8)  |
                               ((in_val & 0x00FF0000) >> 8)  |
                               ((in_val & 0xFF000000) >> 24);
        
        // Call the design function.
        ap_uint<32> out_val;
        TopModule(in_val, out_val);
        
        // Compare the output with the expected value.
        if (out_val != expected) {
            std::cout << "Mismatch at cycle " << cycle 
                      << ": Input = 0x" << std::hex << in_val 
                      << ", Expected = 0x" << expected 
                      << ", Got = 0x" << out_val << std::dec << std::endl;
            errors++;
        }
    }

    // Report test results.
    if (errors == 0) {
        std::cout << "Test Passed: All " << test_cycles << " cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errors << " mismatches detected out of " << test_cycles << " cases." << std::endl;
    }

    return 0;
}
