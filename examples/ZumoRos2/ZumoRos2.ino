// ZumoRos2.ino - serial bridge between the ROS 2 nodes on the PC and the Zumo
// (motor speeds in, encoder counts out). Used by the ZumoRobot-ROS2 project.
// Author: Mutasem Bader, Felix Fritz Biermann

#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega32U4__)
#error "This sketch supports the ATmega328P and the ATmega32U4 only."
#endif

// ZumoMotors from the ZumoShield library drives the motor driver of the Zumo
// Shield (direction pins 7 and 8, PWM on pins 9 and 10) on both the
// Arduino Uno (ATmega328P) and the Arduino Leonardo (ATmega32U4).
#include <ZumoMotors.h>
#include <Zumo328PEncoders.h>

// Packet layout:
//  PC -> Arduino (7 bytes):  STX(0x02) | left_speed(int16 LE) | right_speed(int16 LE) | ctrl | ETX(0x03)
//  Arduino -> PC (10 bytes): STX(0x02) | right_enc(int32 LE) | left_enc(int32 LE) | ETX(0x03)
//
// ctrl 0x11 = normal run (send encoder reply)
// ctrl 0x12 = reset encoders (no reply)
// The motor speeds are applied for every valid packet, whatever ctrl is.
//
// Safety stop: if no valid packet arrives for COMMAND_TIMEOUT_MS, the motors are
// switched off. This covers a crashed PC program, a pulled USB cable and a
// camera that has lost the line.

ZumoMotors motors;
Zumo328PEncoders encoders;

static const uint8_t COMMAND_PACKET_SIZE = 7;  // PC -> Arduino
static const uint8_t ENCODER_PACKET_SIZE = 10; // Arduino -> PC
static const unsigned long COMMAND_TIMEOUT_MS = 500;

static unsigned long lastCommandMs = 0;
static bool motorsActive = false;  // true while a non-zero speed is applied

void setup() {
    Serial.begin(115200);
    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();
}

void loop() {
    // Unsigned subtraction keeps this correct when millis() wraps around.
    if (motorsActive && (unsigned long)(millis() - lastCommandMs) > COMMAND_TIMEOUT_MS) {
        motors.setSpeeds(0, 0);
        motorsActive = false;
    }

    if (Serial.available() < COMMAND_PACKET_SIZE) {
        return;
    }

    if (Serial.read() != 0x02) {
        return;  // not STX: discard this byte and re-sync on the next iteration
    }

    // Read each byte explicitly as uint8_t to avoid sign-extension before shifting
    uint8_t b0 = (uint8_t)Serial.read();
    uint8_t b1 = (uint8_t)Serial.read();
    uint8_t b2 = (uint8_t)Serial.read();
    uint8_t b3 = (uint8_t)Serial.read();
    uint8_t ctrl = (uint8_t)Serial.read();

    if (Serial.read() != 0x03) {
        return;  // ETX missing: discard the packet
    }

    int16_t left_speed  = (int16_t)((uint16_t)b0 | ((uint16_t)b1 << 8));
    int16_t right_speed = (int16_t)((uint16_t)b2 | ((uint16_t)b3 << 8));

    motors.setSpeeds(left_speed, right_speed);
    lastCommandMs = millis();
    motorsActive = (left_speed != 0 || right_speed != 0);

    if (ctrl == 0x11) {
        sendEncoderData();
    } else if (ctrl == 0x12) {
        encoders.getCountsAndResetLeft();
        encoders.getCountsAndResetRight();
    }
}

void sendEncoderData() {
    int32_t rightEncoder = encoders.getCountsRight();
    int32_t leftEncoder  = encoders.getCountsLeft();

    uint8_t packet[ENCODER_PACKET_SIZE];
    packet[0] = 0x02;  // STX

    // Right encoder (bytes 1 to 4, little-endian)
    packet[1] = (uint8_t)(rightEncoder        & 0xFF);
    packet[2] = (uint8_t)((rightEncoder >> 8) & 0xFF);
    packet[3] = (uint8_t)((rightEncoder >> 16) & 0xFF);
    packet[4] = (uint8_t)((rightEncoder >> 24) & 0xFF);

    // Left encoder (bytes 5 to 8, little-endian)
    packet[5] = (uint8_t)(leftEncoder        & 0xFF);
    packet[6] = (uint8_t)((leftEncoder >> 8) & 0xFF);
    packet[7] = (uint8_t)((leftEncoder >> 16) & 0xFF);
    packet[8] = (uint8_t)((leftEncoder >> 24) & 0xFF);

    packet[9] = 0x03;  // ETX

    Serial.write(packet, sizeof(packet));
}
