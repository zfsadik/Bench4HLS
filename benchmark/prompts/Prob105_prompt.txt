Implement the combinational (next-state and output) logic of a Moore state machine with one 1-bit current state input state, one 1-bit input in, one 1-bit reset input areset (active high), one 1-bit next-state output next_state, and one 1-bit output out. The state machine has two states, with the following behavior:

  B (1) --0--> A
  B (1) --1--> B
  A (0) --0--> B
  A (0) --1--> A

If areset is asserted (active high) then next_state is forced to B and out is 1.

The top-level function should have the following prototype:
void TopModule(bool state, bool in, bool areset, bool &next_state, bool &out)
