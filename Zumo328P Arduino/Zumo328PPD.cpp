#include <Zumo328PPID.h>
#include <Arduino.h>

Zumo328PPID::Zumo328PPID(float maxSpeed) : maxSpeed(maxSpeed), leftSpeed(0), rightSpeed(0) {}

void Zumo328PPID::ControlSpeed(uint16_t& measured_position, const uint16_t target_position, float kp, float kd, float deltaT, bool aktiv){

// Our "error" is how far we are away from the center of the line, which corresponds to target_position.
int16_t error = measured_position - target_position;

// Get motor speed difference using proportional and derivative PID terms (the integral term is generally not very useful for line following).  
if(aktiv){
    //currenttime in micorseconds
    long currT = micros(); 

    //deltaT in seconds
    deltaT = ((float) (currT-prevT))/ 1.0e6; 
    prevT = currT;
}

//Control Signal (motor speed difference)
int32_t speedDifference = (int32_t)(kp * error)+(int32_t)(kd * ((error - lastError) / deltaT));

lastError = error;

// Get individual motor speeds.  The sign of speedDifference determines if the robot turns left or right.
this->leftSpeed = constrain((int32_t)maxSpeed + speedDifference, 0, (int32_t)maxSpeed);
this->rightSpeed = constrain((int32_t)maxSpeed - speedDifference, 0, (int32_t)maxSpeed);
}