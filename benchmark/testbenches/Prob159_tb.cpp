#include <iostream>
#include <cstdlib>
#include <cmath>

// DUT prototype
void TopModule(bool rst, float setpoint, float measurement, float &control_out);

int main() {
    int cycleCount = 0;
    int errorCount = 0;
    std::srand(0);
    const float eps = 1e-3f;

    // Reference state for software model
    float integrator_ref      = 0.0f;
    float prev_error_ref      = 0.0f;
    float prev_derivative_ref = 0.0f;

    // Same gains/constants as in DUT
    const float Kp = 1.0f;
    const float Ki = 0.1f;
    const float Kd = 0.01f;
    const float N  = 10.0f;
    const float dt = 0.01f;
    const float inv_dt           = 100.0f;
    const float inv_filter_denom = 1.0f / (1.0f + N * dt);

    // Generate 2 reset cycles + 998 active cycles
    for (int tc = 0; tc < 1000; ++tc) {
        bool rst = (tc < 2);
        float setpoint    = (std::rand() % 2001 - 1000) / 100.0f;
        float measurement = (std::rand() % 2001 - 1000) / 100.0f;

        float dut_out      = 0.0f;
        float expected_out = 0.0f;

        // Compute reference output
        if (rst) {
            integrator_ref      = 0.0f;
            prev_error_ref      = 0.0f;
            prev_derivative_ref = 0.0f;
            expected_out        = 0.0f;
        } else {
            float error = setpoint - measurement;
            float P     = Kp * error;
            integrator_ref += Ki * error * dt;
            float derivative = (error - prev_error_ref) * inv_dt;
            float D = (N * derivative + prev_derivative_ref) * inv_filter_denom;
            prev_error_ref      = error;
            prev_derivative_ref = D;
            expected_out = P + integrator_ref + Kd * D;
        }

        // Invoke DUT
        TopModule(rst, setpoint, measurement, dut_out);

        if (std::fabs(dut_out - expected_out) > eps) {
            std::cout << "Error at test case " << cycleCount
                      << ": inputs (rst, setpoint, measurement) = ("
                      << rst << ", " << setpoint << ", " << measurement << ")"
                      << " expected ctrl = " << expected_out
                      << ", got = " << dut_out << std::endl;
            errorCount++;
        }
        cycleCount++;
    }

    // Report test summary
    if (errorCount == 0) {
        std::cout << "Test Passed: All " << cycleCount
                  << " test cases matched." << std::endl;
    } else {
        std::cout << "Test Failed: " << errorCount
                  << " mismatches detected out of " << cycleCount
                  << " cases." << std::endl;
    }

    return 0;
}
