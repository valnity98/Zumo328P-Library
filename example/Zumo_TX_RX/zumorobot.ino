#if defined(__AVR_ATmega328P__)
  #include <ZumoShield.h>
#elif defined(__AVR_ATmega32U4__)
  #include <Zumo32U4.h>
#endif

#include <Zumo328PEncoders.h>
#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Global Variables
volatile bool dataAvailable = false;
volatile uint8_t buffer[7] = {0}; // Puffer mit Null initialisieren// Puffer für Protokollrahmen: STX, Daten1[byte1 & byte2], Daten2[byte1 & byte2], Checksum, ETX
volatile uint8_t bufferIndex = 0;  // Index für den Puffer

//Define objects from classes
#if defined(__AVR_ATmega328P__)
  ZumoMotors motors_328P;
#elif defined(__AVR_ATmega32U4__)
  Zumo32U4Motors motors_32u4;
#endif
Zumo328PEncoders encoders;


void setup() {
  Serial.begin(9600); 

  // UART RX Complete Interrupt aktivieren je nach Mikrocontroller
  #if defined(__AVR_ATmega328P__)
    UCSR0B |= (1 << RXCIE0);  // RX Complete Interrupt Enable für ATmega328P
  #elif defined(__AVR_ATmega32U4__)
    UCSR1B |= (1 << RXCIE1);  // RX Complete Interrupt Enable für ATmega32U4 (USART1)
  #else
    #error "Mikrocontroller nicht unterstützt. Bitte UART-Initialisierung manuell prüfen."
  #endif
  // Globale Interrupts aktivieren
  sei();  
}

void loop() {
  if (dataAvailable) {
    // Line-following-Funktion aufrufen
    follow_line();
    dataAvailable = false;  // Flag zurücksetzen
  }

  sendEncoderData();
}

// UART RX ISR je nach Mikrocontroller
#if defined(__AVR_ATmega328P__)
ISR(USART_RX_vect) {
  uint8_t receivedByte = UDR0;  // Lesen des empfangenen Bytes
  processReceivedByte(receivedByte);  // Verarbeiten des empfangenen Bytes
}
#elif defined(__AVR_ATmega32U4__)
ISR(USART1_RX_vect) {
  uint8_t receivedByte = UDR1;  // Lesen des empfangenen Bytes (ATmega32U4)
  processReceivedByte(receivedByte);  // Verarbeiten des empfangenen Bytes 
}
#endif

void processReceivedByte(uint8_t byte) {
  if (bufferIndex == 0 && byte != 0x02) {
    // Auf Start-Byte (STX) warten
    return;
  }

  buffer[bufferIndex++] = byte;

  if (bufferIndex >= sizeof(buffer)) {
    bufferIndex = 0;

    // Prüfen, ob das letzte Byte das End-Byte (ETX) ist
    if (buffer[6] == 0x03 && isChecksumValid(buffer)) {
      dataAvailable = true;
    }
  }
}

bool isChecksumValid(uint8_t* data) {
  uint8_t checksum = data[1] ^ data[2] ^ data[3] ^ data[4]; // XOR der Datenbytes
  return (checksum == data[5]); // Vergleichen mit der empfangenen Prüfsumme
}


// Encoder-Daten senden 
void sendEncoderData() {
int16_t leftEncoder = encoders.getCountsLeft();
int16_t rightEncoder = encoders.getCountsRight();
uint8_t checksum = static_cast<uint8_t>(leftEncoder ^ rightEncoder);  // Beispiel-Checksumme

uint8_t dataToSend[7]; // Array für 8-Bit-Werte

// STX-Byte hinzufügen
dataToSend[0] = 0x02;

// Encoder-Daten in das Array speichern (LSB und MSB)
dataToSend[1] = (uint8_t)(leftEncoder & 0xFF);        // LSB des linken Encoders
dataToSend[2] = (uint8_t)((leftEncoder >> 8) & 0xFF); // 2. Byte des linken Encoders
dataToSend[3] = (uint8_t)(rightEncoder & 0xFF);       // LSB des rechten Encoders
dataToSend[4] = (uint8_t)((rightEncoder >> 8) & 0xFF);// 2. Byte des rechten Encoders

// Prüfziffer hinzufügen
dataToSend[5] = checksum;

// ETX-Byte hinzufügen (Ende des Textes)
dataToSend[6] = 0x03; // ETX

// Senden des gesamten Arrays über die serielle Schnittstelle
Serial.write(dataToSend, sizeof(dataToSend));
}


// Unterschiedliche follow_line-Funktionen für verschiedene Mikrocontroller
void follow_line() {
  // Extrahieren der linken und rechten Geschwindigkeiten aus dem Buffer
  int16_t leftSpeed = (static_cast<int16_t>(buffer[2]) << 8) | buffer[1]; // Vorzeichenbehaftete linke Geschwindigkeit
  int16_t rightSpeed = (static_cast<int16_t>(buffer[4]) << 8) | buffer[3]; // Vorzeichenbehaftete rechte Geschwindigkeit

#if defined(__AVR_ATmega328P__)
 motors_328P.setSpeeds(leftSpeed, rightSpeed);
#elif defined(__AVR_ATmega32U4__)
  motors_32u4.setSpeeds(leftSpeed, rightSpeed);
#endif

}







