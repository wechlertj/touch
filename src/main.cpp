#ifndef ARDUINO_H
#include <Arduino.h>
#define ARDUINO_H
#endif
#define MAPPING
#define FILTERING
#include "defines.h"
#include "helper_functions.h"
#include "Mouse.h"
/*
Port mapping: A0 - A3 -> PF7 - PF4
Pin   Port    Color   Position
--------------------------
A0    PF7     blk     x_le
A1    PF6     brn     x_ri
A2    PF5     red     y_up
A3    PF4     ylw     y_lo

*/

int xvals[OVERSAMPLING] {0};
int yvals[OVERSAMPLING] {0};
int *p;

void setup() {
  cli();
  // Setting unused pins as Input and activate pull-ups. (p. 71)
  DDRB = 0x00;
  PORTB = 0xFF;
  DDRC = 0x80;                                              // LED_BUILTIN (PC7) as output
  PORTC = 0x7F;                                             // LED_BUILTIN off
  DDRD = 0x00;
  PORTD = 0xFF;
  DDRE = 0x00;
  PORTE = 0xFF;
  // Panel is connected to PORTF pins PF7:4
  DDRF = 0x80;                                              // All pins as input
  PORTF = 0x20;                                             // All pins no pull-up / LOW

  // Pre-configuring ADC
  ADMUX |= (1<<REFS0) | (1<<MUX2) | (1<<MUX0);  // Set VRef to Vcc and connect ADC5 to ADC (p. 313)
  // ADC enable and ADC-clock prescaler to 128. CPU-Clock / ADC-prescaler = ADC-Clock.
  // At 10bit resolution must not be higher than 200kHz.
  // ADPS2:0 set to 1 prescales by 128 giving an ADC-clock of 125kHz (8us per cycle).
  // Sampling + conversion takes up 1.5+13 cycles -> 12+104us
  // See p. 300 ff
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

  /*
  Configuring interrupts (p. 89)
  ISCn1    ISCn0   Description
  0        0       The low level of INTn generates an interrupt request
  0        1       Any edge of INTn generates asynchronously an interrupt request
  1        0       The falling edge of INTn generates asynchronously an interrupt request
  1        1       The rising edge of INTn generates asynchronously an interrupt request
  */
  EICRA |= (1<<ISC00);
  EIMSK |= (1<<INT0);                                       // Enable external interrupt on INTO

  sei();
  Serial.begin(115200);
  Mouse.begin();

  delay(100);
}

void loop() {
  delay(5);
  float xval;
  float yval;

  if (isFingered()) {

    for (int i = 0; i < OVERSAMPLING; i++) {
      xvals[i] = readX();
    }

    if (isFingered()) {
      for (int i = 0; i < OVERSAMPLING; i++) {
        yvals[i] = readY();
      }

      xval = doSomeMedianFiltering(xvals, OVERSAMPLING, CLAMP);
      yval = doSomeMedianFiltering(yvals, OVERSAMPLING, CLAMP);

      // Mouse control values are only send if MOUSE_EN (PIND6) is pulled LOW,
      // else positional vales are sent over serial
      if (!(PIND & (1<<MOUSE_EN))) {
        xval = myMap(xval, XMIN, XMAX, MOUSE_SPEED, -MOUSE_SPEED);
        yval = myMap(yval, YMIN, YMAX, MOUSE_SPEED, -MOUSE_SPEED);
        Mouse.move(xval, yval, 0);
        delay(MOUSE_DELAY);
      } else {
        #ifdef MAPPING
        xval = myMap(xval, XMIN, XMAX, WIDTH*.5, -WIDTH*.5);
        yval = myMap(yval, YMIN, YMAX, -HEIGHT*.5, HEIGHT*.5);
        #endif
        Serial.print(xval,1);Serial.print("\t");Serial.println(yval,1);
      }
    }
  }
}

ISR(INT0_vect){
  // Let LED_L indicate if mouse control is enabled. ON if turned on, OFF otherwise
  if (PIND & (1<<MOUSE_EN)){
    PORTC &= ~(1<<LED_L);                                   // LED_L off
  } else {
    PORTC |= (1<<LED_L);                                    // LED_L on
  }
}