#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ap_int.h"

// Prototype of the HLS design
void TopModule(
    const ap_uint<8> message[64],
    ap_uint<32>      length,
    ap_uint<32>      digest[5],
    bool            &valid
);

// Reference SHA-1 in C++
static inline uint32_t ROTL32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

void ref_sha1(
    const ap_uint<8> message[64],
    ap_uint<32>      length,
    ap_uint<32>      digest[5],
    bool            &valid
) {
    uint32_t H[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE,
        0x10325476, 0xC3D2E1F0
    };
    uint32_t W[80] = {0};
    uint32_t len = (uint32_t)length;

    // Load & pad
    for (int i = 0; i < 64; i++) {
        uint8_t b = (i < (int)len) ? (uint8_t)message[i] : 0;
        int wi = i >> 2, sh = (3 - (i & 3))*8;
        W[wi] |= (uint32_t)b << sh;
    }
    if (len < 64) {
        int p = (int)len;
        W[p>>2] |= 0x80u << ((3 - (p & 3))*8);
    }
    if (len < 56) W[14] = 0;
    if (len < 60) W[15] = len * 8;

    for (int t = 16; t < 80; t++)
        W[t] = ROTL32(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);

    uint32_t A=H[0], B=H[1], C=H[2], D=H[3], E=H[4];
    for (int t = 0; t < 80; t++) {
        uint32_t f, K;
        if      (t < 20) { f = (B & C) | (~B & D);          K = 0x5A827999; }
        else if (t < 40) { f = B ^ C ^ D;                    K = 0x6ED9EBA1; }
        else if (t < 60) { f = (B & C) | (B & D) | (C & D); K = 0x8F1BBCDC; }
        else             { f = B ^ C ^ D;                    K = 0xCA62C1D6; }
        uint32_t temp = ROTL32(A,5) + f + E + W[t] + K;
        E = D; D = C; C = ROTL32(B,30); B = A; A = temp;
    }
    H[0]+=A; H[1]+=B; H[2]+=C; H[3]+=D; H[4]+=E;
    for (int i = 0; i < 5; i++) digest[i] = H[i];
    valid = true;
}

int main() {
    std::srand((unsigned)std::time(nullptr));
    int cycleCount = 0, errorCount = 0;

    ap_uint<8>   message[64];
    ap_uint<32>  length;
    ap_uint<32>  digest_dut[5], digest_ref[5];
    bool         valid_dut, valid_ref;

    // --- Phase 1: Fixed Tests ---
    struct { const char *str; uint32_t len; } fixed[] = {
        {"",    0},
        {"abc", 3}
    };
    for (auto &tc : fixed) {
        length = tc.len;
        for (int i = 0; i < 64; i++)
            message[i] = (i < (int)tc.len) ? (ap_uint<8>)tc.str[i] : (ap_uint<8>)0;

        TopModule(message, length, digest_dut, valid_dut);
        ref_sha1(message, length, digest_ref, valid_ref);

        for (int i = 0; i < 5; i++) {
            if (digest_dut[i] != digest_ref[i]) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected digest[" << i << "] = "
                          << digest_ref[i]
                          << ", got " << digest_dut[i]
                          << std::endl;
                errorCount++;
            }
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected valid = " << valid_ref
                      << ", got " << valid_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // --- Phase 1b: Boundary Tests (lengths 55, 56, 60, 63) ---
    {
        uint32_t boundary_lens[] = {55, 56, 60, 63};
        for (int bi = 0; bi < 4; bi++) {
            length = boundary_lens[bi];
            for (int i = 0; i < 64; i++)
                message[i] = (ap_uint<8>)(i + 1);

            TopModule(message, length, digest_dut, valid_dut);
            ref_sha1(message, length, digest_ref, valid_ref);

            for (int i = 0; i < 5; i++) {
                if (digest_dut[i] != digest_ref[i]) {
                    std::cout << "Mismatch at cycle " << cycleCount
                              << ": expected digest[" << i << "] = "
                              << digest_ref[i]
                              << ", got " << digest_dut[i]
                              << std::endl;
                    errorCount++;
                }
            }
            if (valid_dut != valid_ref) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected valid = " << valid_ref
                          << ", got " << valid_dut << std::endl;
                errorCount++;
            }
            cycleCount++;
        }
    }

    // --- Phase 2: Random Tests (100 cases) ---
    const int numRandom = 100;
    for (int t = 0; t < numRandom; t++) {
        length = std::rand() % 64;
        for (int i = 0; i < 64; i++)
            message[i] = std::rand() & 0xFF;

        TopModule(message, length, digest_dut, valid_dut);
        ref_sha1(message, length, digest_ref, valid_ref);

        for (int i = 0; i < 5; i++) {
            if (digest_dut[i] != digest_ref[i]) {
                std::cout << "Mismatch at cycle " << cycleCount
                          << ": expected digest[" << i << "] = "
                          << digest_ref[i]
                          << ", got " << digest_dut[i]
                          << std::endl;
                errorCount++;
            }
        }
        if (valid_dut != valid_ref) {
            std::cout << "Mismatch at cycle " << cycleCount
                      << ": expected valid = " << valid_ref
                      << ", got " << valid_dut << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Print final test summary.
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
