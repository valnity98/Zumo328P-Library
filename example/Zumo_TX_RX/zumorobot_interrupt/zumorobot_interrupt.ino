#if defined(__AVR_ATmega328P__)
  #include <ZumoShield.h>
  #define USART_RX_vect USART_RX_vect // Interrupt-Vektor für ATmega328P
#elif defined(__AVR_ATmega32U4__)
  #include <Zumo32U4.h>
  #define USART_RX_vect USART1_RX_vect // Interrupt-Vektor für ATmega32U4
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

// Globale Variablen für den Empfangszustand
volatile uint8_t recvBuffer[5]; // Puffer für 5 Bytes: STX, 2x left_speed, 2x right_speed, ETX
volatile uint8_t recvIndex = 0; // Index für das aktuelle Byte
volatile bool packetComplete = false; // Flag, wenn ein Paket vollständig empfangen wurde

void setup() {
  Serial.begin(115200);

  // UART-Interrupt aktivieren
#if defined(__AVR_ATmega328P__)
  UCSR0B |= (1 << RXCIE0); // RX-Interrupt aktivieren
#elif defined(__AVR_ATmega32U4__)
  UCSR1B |= (1 << RXCIE1); // RX-Interrupt für USART1 aktivieren
#endif
  sei(); // Globale Interrupts aktivieren
}

void loop() {
  // Prüfen, ob ein vollständiges Paket empfangen wurde
  if (packetComplete) {
    // Paket verarbeiten
    if (recvBuffer[0] == 0x02 && recvBuffer[4] == 0x03) { // STX und ETX prüfen
      int16_t left_speed = recvBuffer[1] | (recvBuffer[2] << 8);
      int16_t right_speed = recvBuffer[3] | (recvBuffer[4] << 8);

      // Motoren ansteuern
      #if defined(__AVR_ATmega328P__)
        motors_328P.setSpeeds(left_speed, right_speed);
      #elif defined(__AVR_ATmega32U4__)
        motors_32u4.setSpeeds(left_speed, right_speed);
      #endif

      // Encoder-Daten senden
      sendEncoderData();
    }

    // Flag zurücksetzen
    packetComplete = false;
  }
}

// Interrupt-Service-Routine für UART-Empfang
ISR(USART_RX_vect) {
#if defined(__AVR_ATmega328P__)
  uint8_t data = UDR0; // Empfange Byte aus UART-Register (ATmega328P)
#elif defined(__AVR_ATmega32U4__)
  uint8_t data = UDR1; // Empfange Byte aus UART-Register (ATmega32U4)
#endif

  if (!packetComplete) {
    recvBuffer[recvIndex++] = data;

    if (recvIndex == 5) { // Ein Paket (5 Bytes) ist vollständig
      recvIndex = 0;
      packetComplete = true;
    }
  }
}

// Encoder-Daten senden (für 32-Bit signed)
void sendEncoderData() {
  int32_t leftEncoder = encoders.getCountsLeft();
  int32_t rightEncoder = encoders.getCountsRight();

  uint8_t dataToSend[10]; // Array für 8-Bit-Werte (1 Byte STX, 8 Bytes Daten, 1 Byte ETX)

  // STX-Byte hinzufügen
  dataToSend[0] = 0x02;

  // Encoder-Daten in das Array speichern (LSB bis MSB)
  // Linker Encoder (4 Bytes)
  dataToSend[1] = (uint8_t)(leftEncoder & 0xFF);          // Byte 0 (LSB)
  dataToSend[2] = (uint8_t)((leftEncoder >> 8) & 0xFF);   // Byte 1
  dataToSend[3] = (uint8_t)((leftEncoder >> 16) & 0xFF);  // Byte 2
  dataToSend[4] = (uint8_t)((leftEncoder >> 24) & 0xFF);  // Byte 3 (MSB)

  // Rechter Encoder (4 Bytes)
  dataToSend[5] = (uint8_t)(rightEncoder & 0xFF);         // Byte 0 (LSB)
  dataToSend[6] = (uint8_t)((rightEncoder >> 8) & 0xFF);  // Byte 1
  dataToSend[7] = (uint8_t)((rightEncoder >> 16) & 0xFF); // Byte 2
  dataToSend[8] = (uint8_t)((rightEncoder >> 24) & 0xFF); // Byte 3 (MSB)

  // ETX-Byte hinzufügen (Ende des Textes)
  dataToSend[9] = 0x03;

  // Senden des gesamten Arrays über die serielle Schnittstelle
  Serial.write(dataToSend, sizeof(dataToSend));
}
