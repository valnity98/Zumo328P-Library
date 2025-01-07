
/* This program shows how to read the encoders on the Zumo 32U4.
The encoders can tell you how far, and in which direction each
motor has turned.
*/

#include <Zumo328PEncoders.h>
#include <ZumoShield.h>

Zumo328PEncoders encoders;
ZumoMotors motors;

char report[80];

void setup()
{
    Serial.begin(9600);
}

void loop()
{

    int32_t countsLeft = encoders.getCountsLeft();
    int32_t countsRight = encoders.getCountsRight();

    // Send the information to the serial monitor also.  
    Serial.print(countsLeft);
    Serial.print(" , "); 
    Serial.print(countsRight);
    Serial.println();
  

    motors.setSpeeds(100, 100);
    delay(2);
}
