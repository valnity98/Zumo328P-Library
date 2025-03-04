#ifndef Zumo328PPID_H
#define Zumo328PPID_H

#include <Arduino.h>

class Zumo328PPID {
public:
    Zumo328PPID(float maxSpeed = 200.0);  // Konstruktor mit optionaler maxSpeed

    void ControlSpeed(uint16_t& measured_position, uint16_t target_position, float kp, float kd , float deltaT = 1, bool aktiv = true);

    int32_t getLeftSpeed() { return leftSpeed; }  // Getter für leftSpeed
    int32_t getRightSpeed() { return rightSpeed; }  // Getter für rightSpeed

private:
    float maxSpeed;  // Maximale Geschwindigkeit
    int32_t leftSpeed;
    int32_t rightSpeed;

    long prevT = 0;  // Vorherige Zeit (Mikrosekunden)
    int16_t lastError = 0;  // Der letzte Fehler
    
};

#endif
