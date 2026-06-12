void TopModule(bool rst, float setpoint, float measurement, float &control_out){
	#pragma HLS INTERFACE ap_none port=rst
    #pragma HLS INTERFACE ap_none port=setpoint
    #pragma HLS INTERFACE ap_none port=measurement
    #pragma HLS INTERFACE ap_none port=control_out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

	static float integrator     = 0.0f;
    static float prev_error     = 0.0f;
    static float prev_derivative = 0.0f;

    const float Kp = 1.0f;
    const float Ki = 0.1f;
    const float Kd = 0.01f;
    const float N  = 10.0f;
    const float dt = 0.01f;
    const float inv_dt           = 100.0f;
    const float inv_filter_denom = 1.0f / (1.0f + N * dt);

    if (rst) {
        integrator      = 0.0f;
        prev_error      = 0.0f;
        prev_derivative = 0.0f;
        control_out     = 0.0f;
    } else {
        float error = setpoint - measurement;

        float P = Kp * error;

        integrator += Ki * error * dt;

        float derivative = (error - prev_error) * inv_dt;

        float D = (N * derivative + prev_derivative) * inv_filter_denom;

        prev_error      = error;
        prev_derivative = D;

        control_out = P + integrator + Kd * D;
    }
}
