#include <iostream>
#include <cstdlib>

// DUT prototype
void TopModule(const unsigned char data[256], unsigned int length, unsigned int &crc_out);

// Reference CRC-32
unsigned int referenceCRC(const unsigned char data[256],
                          unsigned int       length)
{
    unsigned int crc = 0xFFFFFFFFu;
    for (unsigned int i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 1u)
                crc = (crc >> 1) ^ 0xEDB88320u;
            else
                crc =  crc >> 1;
        }
    }
    return ~crc;
}

int main() {
    std::srand(0);
    int cycleCount = 0;
    int errorCount = 0;

    unsigned char data[256];
    unsigned int length;
    unsigned int dut_crc, expected_crc;

    // 100 random test cases
    for (int tc = 0; tc < 100; ++tc) {
        length = std::rand() % 256;
        for (unsigned int i = 0; i < length; ++i)
            data[i] = static_cast<unsigned char>(std::rand() % 256);

        expected_crc = referenceCRC(data, length);
        TopModule(data, length, dut_crc);

        if (dut_crc != expected_crc) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (length, data[0]) = ("
                      << length << ", "
                      << static_cast<unsigned>(data[0]) << ")"
                      << " expected crc = " << expected_crc
                      << ", got crc = " << dut_crc << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Maximum-length test case (length == 256)
    length = 256;
    for (unsigned int i = 0; i < 256; ++i)
        data[i] = static_cast<unsigned char>(std::rand() % 256);
    expected_crc = referenceCRC(data, length);
    TopModule(data, length, dut_crc);
    if (dut_crc != expected_crc) {
        std::cout << "Error at test case " << cycleCount
                  << ": inputs (length, data[0]) = ("
                  << length << ", "
                  << static_cast<unsigned>(data[0]) << ")"
                  << " expected crc = " << expected_crc
                  << ", got crc = " << dut_crc << std::endl;
        errorCount++;
    }
    cycleCount++;

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
