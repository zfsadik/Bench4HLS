Consider the function f shown in the Karnaugh map below. Implement this function for HLS.

           x[3]x[4]
x[1]x[2]   00  01  11  10
   00     |  1 |  0 |  0 |  1 |
   01     |  0 |  0 |  0 |  0 |
   11     |  1 |  1 |  1 |  0 |
   10     |  1 |  1 |  0 |  1 |

The top-level function should have the following prototype:
void TopModule(ap_uint<4> x, ap_uint<1> &f)