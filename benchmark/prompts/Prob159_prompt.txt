Design a classic PID control loop with a first-order filter on the derivative term. On every invocation, it either resets its internal state or computes a new control output based on the difference between the desired setpoint and the measurement value. When rst=1, all internal state registers and the output register are synchronously cleared to 0.

Internally, the process unfolds as a small datapath of adders, multipliers, and registers: an accumulator for the I-term, a difference block for the D-term, a simple IIR filter on that difference, and registers to hold the previous error and filtered derivative. The final sum drives the control output.

Parameter values:
	Kp = 1.0f
	Ki = 0.1f
	Kd = 0.01f
	N  = 10.0f   [derivative filter coefficient]
	dt = 0.01f   [sample period]

The design is pipelined with II=1, meaning it accepts one new sample per clock cycle. Due to floating-point operation latency, the actual output latency spans multiple cycles, but throughput remains one sample per cycle.

The top-level function should have the following prototype:
void TopModule(bool rst, float setpoint, float measurement, float &control_out)
