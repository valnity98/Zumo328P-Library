#include <ZumoShield.h>
#include <Zumo328PEncoders.h>

// Global Variables
volatile bool dataAvailable = false;
volatile int8_t buffer[2];

//Define objects from classes
ZumoMotors motors;
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

// UART RX ISR je nach Mikrocontroller
#if defined(__AVR_ATmega328P__)
ISR(USART_RX_vect) {
  static uint8_t index = 0;
  buffer[index++] = UDR0;  // UART-Daten lesen (ATmega328P)
  if (index >= 2) {  // Wenn 2 Bytes empfangen wurden
    index = 0;  // Index zurücksetzen
    dataAvailable = true;  // Daten sind verfügbar  
}
#elif defined(__AVR_ATmega32U4__)
ISR(USART1_RX_vect) {
  static uint8_t index = 0;
  buffer[index++] = UDR1;  // UART-Daten lesen ATmega32U4
  if (index >= 2) {  // Wenn 2 Bytes empfangen wurden
    index = 0;  // Index zurücksetzen
    dataAvailable = true;  // Daten sind verfügbar  
}
#endif

void loop() {
  if (dataAvailable) {

    // Line-following-Funktion aufrufen
    follow_line();

    dataAvailable = false;  // Flag zurücksetzen
  }
}

void follow_line() {
  motors.setSpeeds(buffer[0], buffer[1]);
}
