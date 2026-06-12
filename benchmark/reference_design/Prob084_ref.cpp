void TopModule(bool a, bool b,
               bool &out_and,
               bool &out_or,
               bool &out_xor,
               bool &out_nand,
               bool &out_nor,
               bool &out_xnor,
               bool &out_anotb){
				   
	#pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS INTERFACE ap_none port=out_anotb
    #pragma HLS INTERFACE ap_none port=out_xnor
    #pragma HLS INTERFACE ap_none port=out_nor
    #pragma HLS INTERFACE ap_none port=out_nand
    #pragma HLS INTERFACE ap_none port=out_xor
    #pragma HLS INTERFACE ap_none port=out_or
    #pragma HLS INTERFACE ap_none port=out_and
    #pragma HLS INTERFACE ap_none port=b
    #pragma HLS INTERFACE ap_none port=a
	
    out_and   = a && b;
    out_or    = a || b;
    out_xor   = a ^ b;
    out_nand  = !(a && b);
    out_nor   = !(a || b);
    out_xnor  = (a == b);
    out_anotb = a && (!b);
}
