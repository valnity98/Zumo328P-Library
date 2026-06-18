#ifndef Zumo328PPID_H
#define Zumo328PPID_H

#include <Arduino.h>

class Zumo328PPID {
public:
    Zumo328PPID(float maxSpeed = 200.0);

    void ControlSpeed(uint16_t measured_position, uint16_t target_position,
                      float kp, float kd, float deltaT = 1, bool aktiv = true);

    int32_t getLeftSpeed()  { return leftSpeed; }
    int32_t getRightSpeed() { return rightSpeed; }

private:
    float   maxSpeed;
    int32_t leftSpeed;
    int32_t rightSpeed;

    long    prevT     = 0;
    bool    firstCall = true;  // skip spurious large deltaT on very first call
    int16_t lastError = 0;
};

#endif
