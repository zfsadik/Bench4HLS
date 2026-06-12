#include <iostream>
#include "ap_int.h"

// Declaration of the TopModule function.
void TopModule(ap_uint<1>& out);

int main() {
    int errors = 0;

    ap_uint<1> out;
    TopModule(out);

    if (out != 0) {
        std::cout << "Error: Expected 0, got " << out << std::endl;
        errors++;
    }

    if (errors == 0) {
        std::cout << "Test Passed." << std::endl;
    } else {
        std::cout << "Test Failed: " << errors << " mismatches detected." << std::endl;
    }

    return 0;
}
