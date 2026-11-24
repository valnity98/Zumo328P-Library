// Zumo328PPID.cpp - PD controller for line following (steering from lateral position error)
// Author: Mutasem Bader, Felix Fritz Biermann
#include <Zumo328PPID.h>
#include <Arduino.h>

Zumo328PPID::Zumo328PPID(float maxSpeed)
    : maxSpeed(maxSpeed), leftSpeed(0), rightSpeed(0) {}

// Computes left/right motor speeds around the base speed maxSpeed.
// With measureTime == true, deltaT is measured with micros(); otherwise the
// deltaT argument is used as given.
void Zumo328PPID::ControlSpeed(uint16_t measured_position,
                                uint16_t target_position,
                                float kp, float kd,
                                float deltaT, bool measureTime) {

    // 32-bit arithmetic: the difference of two uint16_t values does not fit
    // into an int16_t.
    int32_t error = (int32_t)measured_position - (int32_t)target_position;

    if (measureTime) {
        unsigned long currT = micros();
        deltaT = (float)(currT - prevT) / 1.0e6f;  // unsigned: survives micros() wrap
        prevT  = currT;
    }

    // Derivative term. On the first call there is no previous sample, and a
    // time step of zero (two calls within the same microsecond) would divide
    // by zero; in both cases the term is left out.
    float derivative = 0.0f;
    if (!firstCall && deltaT > 0.0f) {
        derivative = (float)(error - lastError) / deltaT;
    }
    firstCall = false;
    lastError = error;

    // Limit the correction before converting it to an integer. A float that
    // is out of range for int32_t would give an undefined result.
    float correction = constrain(kp * (float)error + kd * derivative,
                                 -2.0f * maxSpeed, 2.0f * maxSpeed);
    int32_t speedDifference = (int32_t)correction;

    // Negative speeds are allowed so the robot can counter-steer in sharp curves.
    this->leftSpeed  = constrain((int32_t)maxSpeed + speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
    this->rightSpeed = constrain((int32_t)maxSpeed - speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
}
