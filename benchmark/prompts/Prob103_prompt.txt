Implement a Moore state machine (combinational portion only) with one 1-bit input in, one 1-bit output out, and a 1-bit current state signal state. The state machine is defined by the following:

  B (out=1) --in=0--> A
  B (out=1) --in=1--> B
  A (out=0) --in=0--> B
  A (out=0) --in=1--> A

Synchronous reset is active high and forces the state to B. Write the function that computes the next-state and output given the current state and input.

The top-level function should have the following prototype:
void TopModule(bool in, bool state, bool reset, bool &next_state, bool &out)
