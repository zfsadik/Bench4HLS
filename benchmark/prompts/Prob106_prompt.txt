This is a Moore state machine with two states, two inputs, and one output. Implement this state machine in for HLS. Reset is an active-high synchronous reset to state OFF. The machine has the following behavior:

  OFF (out=0) --j=0--> OFF
  OFF (out=0) --j=1--> ON
  ON  (out=1) --k=0--> ON
  ON  (out=1) --k=1--> OFF

The top-level function should have the following prototype:
void TopModule(bool j, bool k, bool reset, bool &out)