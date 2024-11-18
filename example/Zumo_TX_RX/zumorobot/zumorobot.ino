#if defined(__AVR_ATmega328P__)
  #include <ZumoShield.h>
#elif defined(__AVR_ATmega32U4__)
  #include <Zumo32U4.h>
#endif

#include <Zumo328PEncoders.h>
#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Define objects from classes
#if defined(__AVR_ATmega328P__)
  ZumoMotors motors_328P;
#elif defined(__AVR_ATmega32U4__)
  Zumo32U4Motors motors_32u4;
#endif
Zumo328PEncoders encoders;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Prüfe, ob genügend Bytes verfügbar sind (mindestens 5 Bytes: 1 Start-Byte, 4 Datenbytes, 1 End-Byte)
  if (Serial.available() >= 5) {
    // Lese das Start-Byte (STX - 0x02)
    if (Serial.read() == 0x02) {
      // Lese die nächsten 4 Bytes für left_speed und right_speed
      int16_t left_speed = Serial.read() | (Serial.read() << 8);
      int16_t right_speed = Serial.read() | (Serial.read() << 8);

      // Setze die Geschwindigkeiten der Motoren
      #if defined(__AVR_ATmega328P__)
        motors_328P.setSpeeds(left_speed, right_speed);
      #elif defined(__AVR_ATmega32U4__)
        motors_32u4.setSpeeds(left_speed, right_speed);
      #endif

      // Lese das End-Byte (ETX - 0x03)
      if (Serial.read() == 0x03) {
        sendEncoderData();
      }
    }
  }
}

// Encoder-Daten senden
void sendEncoderData() {
  int16_t leftEncoder = encoders.getCountsLeft();
  int16_t rightEncoder = encoders.getCountsRight();

  uint8_t dataToSend[6]; // Array für 8-Bit-Werte

  // STX-Byte hinzufügen
  dataToSend[0] = 0x02;

  // Encoder-Daten in das Array speichern (LSB und MSB)
  dataToSend[1] = (uint8_t)(leftEncoder & 0xFF);        // LSB des linken Encoders
  dataToSend[2] = (uint8_t)((leftEncoder >> 8) & 0xFF); // 2. Byte des linken Encoders
  dataToSend[3] = (uint8_t)(rightEncoder & 0xFF);       // LSB des rechten Encoders
  dataToSend[4] = (uint8_t)((rightEncoder >> 8) & 0xFF);// 2. Byte des rechten Encoders

  // ETX-Byte hinzufügen (Ende des Textes)
  dataToSend[5] = 0x03; // ETX

  // Senden des gesamten Arrays über die serielle Schnittstelle
  Serial.write(dataToSend, sizeof(dataToSend));
}
