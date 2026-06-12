Rule 110 is a one-dimensional cellular automaton with interesting properties (such as being Turing-complete). There is a one-dimensional array of cells (on or off). At each time step, the state of each cell changes. In Rule 110, the next state of each cell depends only on itself and its two neighbours, according to the following table:

  Left | Center | Right | Center's next state
  1    | 1      | 1     | 0
  1    | 1      | 0     | 1
  1    | 0      | 1     | 1
  1    | 0      | 0     | 0
  0    | 1      | 1     | 1
  0    | 1      | 0     | 1
  0    | 0      | 1     | 1
  0    | 0      | 0     | 0

The 512-bit register q holds the cell states, with bit 0 at the right end and bit 511 at the left end. For cell i, the left neighbour is bit i+1 (higher index) and the right neighbour is bit i-1 (lower index).

In this circuit, create a 512-cell system (512-bit register named q), and advance by one time step each clock cycle. The synchronous active-high load input indicates the state of the system should be loaded with the 512-bit input (data) on that cycle; otherwise the automaton advances one step. The output q always reflects the current register state after the operation. Assume the boundaries (the left neighbour of bit 511 and the right neighbour of bit 0) are both zero (off).

The top-level function should have the following prototype:
void TopModule(bool load, ap_uint<512> data, ap_uint<512> &q)
