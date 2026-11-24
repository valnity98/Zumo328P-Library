/* This program shows how to read the encoders on the Zumo Shield.
The encoders can tell you how far, and in which direction each
motor has turned.

The robot drives forward while the sketch runs. Lift it up or put it
on a free surface before you power it.
*/

#include <Zumo328PEncoders.h>
#include <ZumoMotors.h>

Zumo328PEncoders encoders;
ZumoMotors motors;

void setup()
{
    Serial.begin(9600);
    motors.setSpeeds(100, 100);
}

void loop()
{
    int32_t countsLeft = encoders.getCountsLeft();
    int32_t countsRight = encoders.getCountsRight();

    // Print the counts to the serial monitor.
    Serial.print(countsLeft);
    Serial.print(" , ");
    Serial.print(countsRight);
    Serial.println();

    delay(2);
}
