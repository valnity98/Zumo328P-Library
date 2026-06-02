#if defined(__AVR_ATmega328P__)
#include <ZumoShield.h>
#elif defined(__AVR_ATmega32U4__)
#include <Zumo32U4.h>
#endif

#include <Zumo328PEncoders.h>
#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Packet layout (7 bytes TX from PC / 10 bytes RX to PC):
//  TX: STX(0x02) | left_speed(int16 LE) | right_speed(int16 LE) | ctrl(0x11/0x12) | ETX(0x03)
//  RX: STX(0x02) | right_enc(int32 LE) | left_enc(int32 LE) | ETX(0x03)
//
// ctrl 0x11 = normal run (send encoder reply)
// ctrl 0x12 = reset encoders (no reply)

#if defined(__AVR_ATmega328P__)
ZumoMotors motors_328P;
#elif defined(__AVR_ATmega32U4__)
Zumo32U4Motors motors_32u4;
#endif

Zumo328PEncoders encoders;

static const uint8_t TX_PACKET_SIZE = 7;

void setup() {
    Serial.begin(115200);
    encoders.getCountsAndResetLeft();
    encoders.getCountsAndResetRight();
}

void loop() {
    if (Serial.available() < TX_PACKET_SIZE) {
        return;
    }

    if (Serial.read() != 0x02) {
        return;  // discard byte if not STX — re-sync on next iteration
    }

    // Read each byte explicitly as uint8_t to avoid sign-extension before shifting
    uint8_t b0 = (uint8_t)Serial.read();
    uint8_t b1 = (uint8_t)Serial.read();
    uint8_t b2 = (uint8_t)Serial.read();
    uint8_t b3 = (uint8_t)Serial.read();
    uint8_t ctrl = (uint8_t)Serial.read();

    if (Serial.read() != 0x03) {
        return;  // ETX missing — discard
    }

    int16_t left_speed  = (int16_t)((uint16_t)b0 | ((uint16_t)b1 << 8));
    int16_t right_speed = (int16_t)((uint16_t)b2 | ((uint16_t)b3 << 8));

#if defined(__AVR_ATmega328P__)
    motors_328P.setSpeeds(left_speed, right_speed);
#elif defined(__AVR_ATmega32U4__)
    motors_32u4.setSpeeds(left_speed, right_speed);
#endif

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

    uint8_t packet[10];
    packet[0] = 0x02;  // STX

    // Right encoder (bytes 1–4, little-endian)
    packet[1] = (uint8_t)(rightEncoder        & 0xFF);
    packet[2] = (uint8_t)((rightEncoder >> 8) & 0xFF);
    packet[3] = (uint8_t)((rightEncoder >> 16) & 0xFF);
    packet[4] = (uint8_t)((rightEncoder >> 24) & 0xFF);

    // Left encoder (bytes 5–8, little-endian)
    packet[5] = (uint8_t)(leftEncoder        & 0xFF);
    packet[6] = (uint8_t)((leftEncoder >> 8) & 0xFF);
    packet[7] = (uint8_t)((leftEncoder >> 16) & 0xFF);
    packet[8] = (uint8_t)((leftEncoder >> 24) & 0xFF);

    packet[9] = 0x03;  // ETX

    Serial.write(packet, sizeof(packet));
}
