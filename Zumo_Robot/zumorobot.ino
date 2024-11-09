#include <ZumoShield.h>
#include <Zumo328PEncoders.h>
#include <Zumo328PPID.h>

#define Kamera_THRESHOLD 1500 

// Global Variables
volatile bool dataAvailable = false;
uint8_t buffer[2];
int16_t received_X = 0;

ZumoMotors motors;
Zumo328PEncoders encoders;
Zumo328PPID PID;

void setup() {
  Serial.begin(9600); 
  UCSR0B |= (1 << RXCIE0);  // UART RX Interrupt aktivieren
  sei();  // Globalen Interrupt aktivieren
}

// UART Empfangs-Interrupt-Service-Routine
ISR(USART_RX_vect) {
  static uint8_t index = 0;
  buffer[index++] = UDR0;  // UART-Daten lesen
  if (index >= 2) {  // Wenn 2 Bytes empfangen wurden
    index = 0;  // Index zurücksetzen
    dataAvailable = true;  // Daten sind verfügbar
  }
}

void loop() {
  if (dataAvailable) {
    // Reconstruct the 16-bit number in little-endian format
    received_X = buffer[1] << 8 | buffer[0];

    // Debugging-Ausgabe
    Serial.print("Received Data: ");
    Serial.println(received_X);

    // Line-following-Funktion aufrufen
    follow_line();

    dataAvailable = false;  // Flag zurücksetzen
  }

  // Hier kannst du weitere nicht-blockierende Aufgaben erledigen
}

void follow_line() {
  motors.setSpeeds(MP.m1_speed, MP.m2_speed);
}
