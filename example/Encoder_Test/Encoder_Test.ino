
/* This program shows how to read the encoders on the Zumo 32U4.
The encoders can tell you how far, and in which direction each
motor has turned.
*/

#include <Wire.h>
#include <Zumo328PEncoders.h>
#include <ZumoShield.h>

Zumo328PEncoders encoders;
ZumoMotors motors;

char report[80];

void setup()
{

}

void loop()
{

    int16_t countsLeft = encoders.getCountsLeft();
    int16_t countsRight = encoders.getCountsRight();

    bool errorLeft = encoders.checkErrorLeft();
    bool errorRight = encoders.checkErrorRight();

 
    // Send the information to the serial monitor also.
    snprintf_P(report, sizeof(report),
        PSTR("%6d %6d %1d %1d"),
        countsLeft, countsRight, errorLeft, errorRight);
    Serial.println(report);
  

    motors.setSpeeds(200, 200);
    delay(2);
}
