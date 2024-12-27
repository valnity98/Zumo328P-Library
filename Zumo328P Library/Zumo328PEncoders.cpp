#include <Zumo328PEncoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define LEFT_A   2 // Changed from 8 to 2 (Pin 2 on Arduino Uno)
#define LEFT_B     6 // Changed from E2 to 6 (Pin 6 on Arduino Uno)
#define RIGHT_A  3 // Changed from 7 to 3 (Pin 3 on Arduino Uno)
#define RIGHT_B    12  // Changed from 23 to 12 (Pin 12 on Arduino Uno)

static volatile bool lastLeftA;
static volatile bool lastRightA;
static volatile bool lastRightB;
static volatile bool lastLeftB;


// These count variables are uint16_t instead of int32_t because
// signed integer overflow is undefined behavior in C++.
static volatile int32_t countLeft;
static volatile int32_t countRight;

static void leftISR() {
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_A>::isInputHigh();

   if (newLeftA != lastLeftA || newLeftB != lastLeftB) {
        // Vorwärts: A wechselt und A != B, Rückwärts: A wechselt und A == B
        if ((newLeftA == lastLeftB) != (newLeftB == lastLeftA)) {
            countLeft++;
        } else {
            countLeft--;
        }
    }

    lastLeftA = newLeftA;
    lastLeftB = newLeftB;
}

static void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_A>::isInputHigh();

    if (newRightA != lastRightA || newRightB != lastRightB) {
        // Vorwärts: A wechselt und A != B, Rückwärts: A wechselt und A == B
        if ((newRightA == lastRightB) != (newRightB == lastRightA)) {
            countRight++;
        } else {
            countRight--;
        }
    }

    lastRightA = newRightA;
    lastRightB = newRightB;
}

void Zumo328PEncoders::init2()
{
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_A>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_A>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();

    // Enable interrupt on PD2 and PD3 for the right encoder and left encoder.  We use attachInterrupt
    // instead of defining ISR(INTx_vect) ourselves so that this class will be
    // compatible with other code that uses attachInterrupt.
    
    attachInterrupt(digitalPinToInterrupt(LEFT_A), leftISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_A), rightISR, CHANGE);

    // Initialize the variables.  It's good to do this after enabling the
    // interrupts in case the interrupts fired by accident as we were enabling
    // them.
    lastLeftA = FastGPIO::Pin<LEFT_A>::isInputHigh();
    lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    countLeft = 0;
    lastRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();	
    lastRightA = FastGPIO::Pin<RIGHT_A>::isInputHigh();
    countRight = 0;
}

int32_t Zumo328PEncoders::getCountsLeft()
{
    init();

    cli();
    int32_t counts = countLeft;
    sei();
    return counts;
}

int32_t Zumo328PEncoders::getCountsRight()
{
    init();

    cli();
    int32_t counts = countRight;
    sei();
    return counts;
}

int32_t Zumo328PEncoders::getCountsAndResetLeft()
{
    init();

    cli();
    int32_t counts = countLeft;
    countLeft = 0;
    sei();
    return counts;
}

int32_t Zumo328PEncoders::getCountsAndResetRight()
{
    init();

    cli();
    int32_t counts = countRight;
    countRight = 0;
    sei();
    return counts;
}




