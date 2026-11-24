// Zumo328PPID.h - PD controller for line following (steering from lateral position error)
// Author: Mutasem Bader, Felix Fritz Biermann
#ifndef Zumo328PPID_H
#define Zumo328PPID_H

#include <Arduino.h>

/*! \brief Discrete-time PD controller that turns a line position error into
 *  left and right motor speeds.
 *
 *  The class keeps its historical name, but it has no integral term. */
class Zumo328PPID {
public:
    /*! \param maxSpeed Base speed of both motors at zero error. It is also
     *  the limit: the outputs stay within [-maxSpeed, maxSpeed]. */
    Zumo328PPID(float maxSpeed = 200.0);

    /*! Computes new motor speeds from the position error.
     *
     *  \param measured_position Measured line position.
     *  \param target_position   Position at which the line should be.
     *  \param kp                Proportional gain.
     *  \param kd                Derivative gain.
     *  \param deltaT            Time step in seconds. Only used when
     *                           measureTime is false.
     *  \param measureTime       If true, the time step is measured with
     *                           micros() and deltaT is ignored. */
    void ControlSpeed(uint16_t measured_position, uint16_t target_position,
                      float kp, float kd, float deltaT = 1, bool measureTime = true);

    int32_t getLeftSpeed()  { return leftSpeed; }
    int32_t getRightSpeed() { return rightSpeed; }

private:
    float   maxSpeed;
    int32_t leftSpeed;
    int32_t rightSpeed;

    unsigned long prevT = 0;
    bool    firstCall = true;  // no previous sample yet: no derivative term
    int32_t lastError = 0;
};

#endif
