// Zumo328PPID.cpp - PD controller for line following (steering from lateral position error)
// Author: Mutasem Bader, Felix Fritz Biermann
#include <Zumo328PPID.h>
#include <Arduino.h>

Zumo328PPID::Zumo328PPID(float maxSpeed)
    : maxSpeed(maxSpeed), leftSpeed(0), rightSpeed(0) {}

// Computes left/right motor speeds around the base speed maxSpeed.
// With aktiv == true, deltaT is measured with micros(); otherwise the
// deltaT argument is used as given.
void Zumo328PPID::ControlSpeed(uint16_t measured_position,
                                uint16_t target_position,
                                float kp, float kd,
                                float deltaT, bool aktiv) {

    int16_t error = (int16_t)measured_position - (int16_t)target_position;

    if (aktiv) {
        unsigned long currT = micros();
        if (firstCall) {
            // No valid previous sample yet: avoid a derivative spike.
            prevT     = currT;
            firstCall = false;
            lastError = error;
            deltaT    = 1e-6f;
        } else {
            deltaT = (float)(currT - prevT) / 1.0e6f;  // unsigned: survives micros() wrap
            prevT  = currT;
        }
    }

    // Two calls within the same microsecond would divide by zero.
    if (deltaT <= 0.0f) {
        deltaT = 1e-6f;
    }

    int32_t speedDifference =
        (int32_t)(kp * error) +
        (int32_t)(kd * ((error - lastError) / deltaT));

    lastError = error;

    // Negative speeds are allowed so the robot can counter-steer in sharp curves.
    this->leftSpeed  = constrain((int32_t)maxSpeed + speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
    this->rightSpeed = constrain((int32_t)maxSpeed - speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
}
