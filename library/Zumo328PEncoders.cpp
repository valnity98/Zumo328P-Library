// Zumo328PEncoders.cpp - interrupt-driven quadrature encoder counting (Zumo Shield, Arduino Leonardo)
// Author: Mutasem Bader, Felix Fritz Biermann
#include <Zumo328PEncoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define LEFT_A   2 // Changed from 8 to 2 (Pin 2 on Arduino Leonardo)
#define LEFT_B     6 // Changed from E2 to 6 (Pin 6 on Arduino Leonardo)
#define RIGHT_A  3 // Changed from 7 to 3 (Pin 3 on Arduino Leonardo)
#define RIGHT_B    12  // Changed from 23 to 12 (Pin 12 on Arduino Leonardo)

static volatile bool lastLeftA;
static volatile bool lastRightA;

// Tick counters, written in the ISRs and read with interrupts disabled.
static volatile int32_t countLeft;
static volatile int32_t countRight;

static void leftISR() {
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

    // Enable interrupt on PD2 and PD3 for the right encoder and left encoder.  We use attachInterrupt
    // instead of defining ISR(INTx_vect) ourselves so that this class will be
    // compatible with other code that uses attachInterrupt.
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




