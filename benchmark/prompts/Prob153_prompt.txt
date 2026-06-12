Design a module that implements a two-stage, decimating FIR filter in a single pipelined datapath, with a 4-phase scheduler that steers samples through stage-0 then stage-1 once every four clock cycles. Inputs are a new fixed-point sample x and a 4-tap coefficient array h[]; outputs are the filtered result y and a valid flag asserted whenever a new decimated output emerges.

For implementing the module, maintain a 2-bit phase counter that increments each clock and wraps every four cycles (values 0->1->2->3->0…).
• Phase 0:
	Shift the new input x into a 4-deep buffer.
	Do a 4-tap dot-product with h[ ].
	Toggle a flip-flop; when the flip-flop becomes true (odd-numbered toggle), latch that sum as y0.
• Phases 1–2: idle.
• Phase 3:
	If y0 was just produced, shift it into a second 4-deep buffer.
	Compute another 4-tap dot-product with h[ ].
	Toggle a second flip-flop; when the flip-flop becomes true (odd-numbered toggle), latch that sum as y and assert valid for one cycle.

All other times valid is low. This delivers one output y (with valid=1) for every four input samples x.

The top-level function should have the following prototype:
void TopModule(ap_fixed<8,1> x, const ap_fixed<8,1> h[4], ap_fixed<28,7> &y, bool &valid)