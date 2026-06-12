void TopModule(const unsigned char data[256], unsigned int length, unsigned int &crc_out){
#pragma HLS INTERFACE ap_none port=data
#pragma HLS INTERFACE ap_none port=length
#pragma HLS INTERFACE ap_none port=crc_out
#pragma HLS INTERFACE ap_ctrl_none port=return

    unsigned int crc = 0xFFFFFFFFu;
    // Process each byte
    for (unsigned int i = 0; i < length; ++i) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=0 max=256
        crc ^= data[i];
        // Process 8 bits
        for (int b = 0; b < 8; ++b) {
            #pragma HLS UNROLL
            if (crc & 1u)
                crc = (crc >> 1) ^ 0xEDB88320u;
            else
                crc =  crc >> 1;
        }
    }
    // Final XOR
    crc_out = ~crc;
}
