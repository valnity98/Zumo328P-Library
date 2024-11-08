#include <Zumo328PEncoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define LEFT_XOR   2 // Changed from 8 to 2 (Pin 2 on Arduino Uno)
#define LEFT_B     6 // Changed from E2 to 6 (Pin 6 on Arduino Uno)
#define RIGHT_XOR  3 // Changed from 7 to 3 (Pin 3 on Arduino Uno)
#define RIGHT_B    12  // Changed from 23 to 12 (Pin 12 on Arduino Uno)

static volatile bool lastLeftA;
static volatile bool lastLeftB;
static volatile bool lastRightA;
static volatile bool lastRightB;

static volatile bool errorLeft;
static volatile bool errorRight;

// These count variables are uint16_t instead of int16_t because
// signed integer overflow is undefined behavior in C++.
static volatile uint16_t countLeft;
static volatile uint16_t countRight;


static void leftISR() {
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ newLeftB;

    countLeft += (newLeftA ^ lastLeftB) - (lastLeftA ^ newLeftB);

    if ((lastLeftA ^ newLeftA) & (lastLeftB ^ newLeftB)) {
        errorLeft = true;
    }

    lastLeftA = newLeftA;
    lastLeftB = newLeftB;
}

static void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ newRightB;

    countRight += (newRightA ^ lastRightB) - (lastRightA ^ newRightB);

    if((lastRightA ^ newRightA) & (lastRightB ^ newRightB))
    {
        errorRight = true;
    }

    lastRightA = newRightA;
    lastRightB = newRightB;
}

void Zumo328PEncoders::init2()
{
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_XOR>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_XOR>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();

    // Enable interrupt on PD2 and PD3 for the right encoder and left encoder.  We use attachInterrupt
    // instead of defining ISR(INTx_vect) ourselves so that this class will be
    // compatible with other code that uses attachInterrupt.
    attachInterrupt(digitalPinToInterrupt(2), leftISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(3), rightISR, CHANGE);

    // Initialize the variables.  It's good to do this after enabling the
    // interrupts in case the interrupts fired by accident as we were enabling
    // them.
    lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    lastLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastLeftB;
    countLeft = 0;
    errorLeft = 0;

    lastRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    lastRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ lastRightB;
    countRight = 0;
    errorRight = 0;
}

int16_t Zumo328PEncoders::getCountsLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    sei();
    return counts;
}

int16_t Zumo328PEncoders::getCountsRight()
{
    init();

    cli();
    int16_t counts = countRight;
    sei();
    return counts;
}

int16_t Zumo328PEncoders::getCountsAndResetLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    countLeft = 0;
    sei();
    return counts;
}

int16_t Zumo328PEncoders::getCountsAndResetRight()
{
    init();

    cli();
    int16_t counts = countRight;
    countRight = 0;
    sei();
    return counts;
}

bool Zumo328PEncoders::checkErrorLeft()
{
    init();

    bool error = errorLeft;
    errorLeft = 0;
    return error;
}

bool Zumo328PEncoders::checkErrorRight()
{
    init();

    bool error = errorRight;
    errorRight = 0;
    return error;
}
