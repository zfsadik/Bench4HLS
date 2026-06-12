#include "ap_int.h"

typedef ap_uint<8>  BYTE;
typedef ap_uint<32> WORD;

static WORD ROTL(WORD x, int n) {
#pragma HLS INLINE
    return (x << n) | (x >> (32 - n));
}

void TopModule(
    const BYTE   message[64],
    ap_uint<32>  length,
    WORD         digest_out[5],
    bool        &valid
) {
#pragma HLS INTERFACE ap_none   port=message
#pragma HLS ARRAY_PARTITION variable=message complete
#pragma HLS INTERFACE ap_none   port=length
#pragma HLS INTERFACE ap_none   port=digest_out
#pragma HLS ARRAY_PARTITION variable=digest_out complete
#pragma HLS INTERFACE ap_none   port=valid
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

    // SHA-1 initial hash values
    WORD H0 = 0x67452301;
    WORD H1 = 0xEFCDAB89;
    WORD H2 = 0x98BADCFE;
    WORD H3 = 0x10325476;
    WORD H4 = 0xC3D2E1F0;

    WORD W[80];
#pragma HLS ARRAY_PARTITION variable=W complete

    // Clear schedule
    for (int t = 0; t < 80; t++) {
        W[t] = 0;
    }

    // Load up to 64 bytes, pad if length < 64
    for (int i = 0; i < 64; i++) {
        BYTE b = (i < length) ? message[i] : (BYTE)0;
        int word_idx = i >> 2;
        int shift    = (3 - (i & 3)) * 8;
        W[word_idx] |= (WORD)b << shift;
    }
    // 0x80 padding bit
    if (length < 64) {
        int pad_pos = length;
        int wi      = pad_pos >> 2;
        int sh      = (3 - (pad_pos & 3)) * 8;
        W[wi] |= (WORD)0x80 << sh;
    }
    // Length in bits
    if (length < 56) W[14] = 0;
    if (length < 60) W[15] = length * 8;

    // Extend schedule
    for (int t = 16; t < 80; t++) {
        W[t] = ROTL(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
    }

    // Working variables
    WORD A = H0, B = H1, C = H2, D = H3, E = H4;

    // Main loop
    for (int t = 0; t < 80; t++) {
        WORD f, K;
        if      (t < 20) { f = (B & C) | (~B & D);          K = 0x5A827999; }
        else if (t < 40) { f = B ^ C ^ D;                    K = 0x6ED9EBA1; }
        else if (t < 60) { f = (B & C) | (B & D) | (C & D); K = 0x8F1BBCDC; }
        else             { f = B ^ C ^ D;                    K = 0xCA62C1D6; }
        WORD temp = ROTL(A,5) + f + E + W[t] + K;
        E = D; D = C; C = ROTL(B,30); B = A; A = temp;
    }

    // Final hash
    H0 += A; H1 += B; H2 += C; H3 += D; H4 += E;
    digest_out[0] = H0;
    digest_out[1] = H1;
    digest_out[2] = H2;
    digest_out[3] = H3;
    digest_out[4] = H4;

    valid = true;
}
