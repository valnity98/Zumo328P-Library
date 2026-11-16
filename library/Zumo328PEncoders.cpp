// Zumo328PEncoders.cpp - interrupt-driven quadrature encoder counting (Zumo Shield, Arduino Leonardo)
// Author: Mutasem Bader, Felix Fritz Biermann
// Derived from Zumo32U4Encoders by Pololu Corporation (MIT License), see THIRD-PARTY-NOTICES.md
#include <Zumo328PEncoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

// Encoder pins on the Zumo Shield (Arduino pin numbers). The Zumo 32U4
// original uses 8/IO_E2 (left) and 7/23 (right).
#define LEFT_A   2   // interrupt pin, left encoder channel A
#define LEFT_B   6   // left encoder channel B (direction)
#define RIGHT_A  3   // interrupt pin, right encoder channel A
#define RIGHT_B  12  // right encoder channel B (direction)

static volatile bool lastLeftA;
static volatile bool lastRightA;

// Tick counters, written in the ISRs and read with interrupts disabled.
static volatile int32_t countLeft;
static volatile int32_t countRight;

// A 32-bit counter cannot be read atomically on an 8-bit AVR, so interrupts
// are disabled around every access. SREG is saved and restored instead of
// calling sei() unconditionally, so these functions do not re-enable
// interrupts when they are called with interrupts already disabled.
static int32_t readCount(volatile int32_t &count, bool reset)
{
    uint8_t oldSREG = SREG;
    cli();
    int32_t counts = count;
    if (reset)
    {
        count = 0;
    }
    SREG = oldSREG;
    return counts;
}

static void leftISR()
{
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_A>::isInputHigh();

    if(newLeftA != lastLeftA){
        if(newLeftA != newLeftB){
            countLeft++;
        } else {
            countLeft--;
        }
    }

    lastLeftA = newLeftA;
}

static void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_A>::isInputHigh();

    if(newRightA != lastRightA){
        if(newRightA != newRightB ){
            countRight--;
        } else {
            countRight++;
        }
    }

    lastRightA = newRightA;
}

void Zumo328PEncoders::init2()
{
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_A>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_A>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();

    // Enable the external interrupts on D2 (left encoder) and D3 (right
    // encoder).  We use attachInterrupt instead of defining ISR(INTx_vect)
    // ourselves so that this class will be compatible with other code that
    // uses attachInterrupt.
    attachInterrupt(digitalPinToInterrupt(LEFT_A), leftISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_A), rightISR, CHANGE);

    // Initialize variables AFTER attaching interrupts. If an interrupt fired
    // accidentally during attachment, countLeft/countRight are reset to 0 here,
    // cleaning up any spurious count. (Original Pololu design intent.)
    lastLeftA  = FastGPIO::Pin<LEFT_A>::isInputHigh();
    countLeft  = 0;
    lastRightA = FastGPIO::Pin<RIGHT_A>::isInputHigh();
    countRight = 0;
}

int32_t Zumo328PEncoders::getCountsLeft()
{
    init();
    return readCount(countLeft, false);
}

int32_t Zumo328PEncoders::getCountsRight()
{
    init();
    return readCount(countRight, false);
}

int32_t Zumo328PEncoders::getCountsAndResetLeft()
{
    init();
    return readCount(countLeft, true);
}

int32_t Zumo328PEncoders::getCountsAndResetRight()
{
    init();
    return readCount(countRight, true);
}
