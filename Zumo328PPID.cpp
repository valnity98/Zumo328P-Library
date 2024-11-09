#include <Zumo328PPID.h>
#include <Arduino.h>

Zumo328PPID::Zumo328PPID(float maxSpeed) : maxSpeed(maxSpeed), leftSpeed(0), rightSpeed(0) {}

void Zumo328PPID::ControlSpeed(uint16_t& measured_position, const uint16_t target_position, float kp, float kd, float ki = 0 ){

// Our "error" is how far we are away from the center of the line, which corresponds to target_position.
int16_t error = measured_position - target_position;

// Get motor speed difference using proportional and derivative PID terms (the integral term is generally not very useful for line following).  

//currenttime in micorseconds
long currT = micros(); 

//deltaT in seconds
float deltaT = ((float) (currT-prevT))/ 1.0e6; 
prevT = currT;


//proportional term
float P_term = kp * error;
//derivative term
float D_term = kd * ((error - lastError) / deltaT);
//integral term
I_term += ki * error * deltaT;

//Control Signal (motor speed difference)
int32_t speedDifference = (int32_t)P_term+(int32_t)D_term+(int32_t)I_term;

lastError = error;

// Get individual motor speeds.  The sign of speedDifference determines if the robot turns left or right.
int32_t leftSpeed = (int32_t)maxSpeed + speedDifference;
int32_t rightSpeed = (int32_t)maxSpeed - speedDifference;

// Constrain our motor speeds to be between 0 and maxSpeed.
// One motor will always be turning at maxSpeed, and the other
// will be at maxSpeed-|speedDifference| if that is positive,
// else it will be stationary.  For some applications, you
// might want to allow the motor speed to go negative so that
// it can spin in reverse.
leftSpeed = constrain(leftSpeed, 0, (int16_t)maxSpeed);
rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);
}