Implement a four‑stage, on‑chip blocked matrix‑multiply engine that reads two input matrices from external memory, multiplies them in a fully pipelined and parallel fashion, and writes the result back out. Its control logic can be thought of as a small finite‑state machine that moves through the phases LOAD_A, LOAD_B, COMPUTE, and WRITE in sequence.

In the LOAD_A phase, the engine streams (size x size) words from the input pointer in1 into a local two‑dimensional buffer A. This brings the entire first matrix on chip so that the subsequent multiply operations can proceed at full internal bandwidth without further external memory accesses.

Next, in the LOAD_B phase, it similarly reads (size x size) words from the second input pointer in2 into a local buffer B. B is partitioned completely along its row dimension, which prepares the design for fully concurrent access to one element from each row simultaneously (i.e., B[col][k] for all values of col at once) when computing dot‑products.

Once both matrices are resident on chip, the design enters the COMPUTE phase. It repeats rep_count independent rounds of the core matrix‑multiply operation (each round recomputes the same A×B product; this is a benchmarking/stress loop, not a mathematical chaining operation).

Finally, in the WRITE phase, the module streams the completed (size x size) output buffer C back out to the pointer out_r in simple row‑major order. This returns the final matrix product to external memory so that the host can read it.

The top-level function should have the following prototype:
void TopModule(int* in1, int* in2, int* out_r, int size, int rep_count)
