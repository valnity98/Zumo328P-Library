#include <Zumo328PPID.h>
#include <Arduino.h>

Zumo328PPID::Zumo328PPID(float maxSpeed)
    : maxSpeed(maxSpeed), leftSpeed(0), rightSpeed(0) {}

// Bug fixes applied:
//   1. Parameter was uint16_t& in .cpp but uint16_t in .h (signature mismatch).
//      Fixed to uint16_t in both.
//   2. prevT=0 on first call caused deltaT = full uptime. Now detected via
//      firstCall flag and skipped on the first tick.
//   3. Speed clamped to [0, maxSpeed] — robot couldn't counter-steer.
//      Fixed to [-maxSpeed, maxSpeed].
void Zumo328PPID::ControlSpeed(uint16_t measured_position,
                                uint16_t target_position,
                                float kp, float kd,
                                float deltaT, bool aktiv) {

    int16_t error = (int16_t)measured_position - (int16_t)target_position;

    if (aktiv) {
        long currT = micros();
        if (firstCall) {
            prevT     = currT;
            firstCall = false;
            lastError = error;
            deltaT    = 1e-6f;
        } else {
            deltaT = (float)(currT - prevT) / 1.0e6f;
            prevT  = currT;
        }
    }

    int32_t speedDifference =
        (int32_t)(kp * error) +
        (int32_t)(kd * ((error - lastError) / deltaT));

    lastError = error;

    this->leftSpeed  = constrain((int32_t)maxSpeed + speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
    this->rightSpeed = constrain((int32_t)maxSpeed - speedDifference,
                                 -(int32_t)maxSpeed, (int32_t)maxSpeed);
}
