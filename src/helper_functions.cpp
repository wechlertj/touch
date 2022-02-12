#include <Arduino.h>
#include "helper_functions.h"

void setTOCM(bool set){
	cli();
	if (set){
		TIMSK0 |= (1<<OCIE0A);
		TCNT0 = 0;
	} else {
		TIMSK0 &= ~(1<<OCIE0A);
	}
	sei();
}

bool isFingered(void){
	/*
	PF7  x_le    LOW
	PF6  x_ri    Hi Z
	PF5  y_up    Hi Z
	PF4  y_lo    ADC
	*/

	DDRF = 0x80;											// Set PF7 as output, others as input
	PORTF = 0x10;											// Pull-up at ADC5 and set PF7 LOW
	ADMUX = 0x44;											// Connect ADC4
	// setTOCM(true);
	int check = getADC();
	if (check < 100) {
		return true;
	} else {
		// Serial.print(check);Serial.println("\t No");
		return false;
	}
}

uint16_t getADC(){
	setTOCM(true);											// Enable Timer Output Compare Match interrupt
	ADCSRA |= (1<<ADSC) | (1<<ADEN);						// Enable ADC and start conversion
	while ((ADCSRA & (1<<ADSC))){							// Wait for Conversion to complete (about 13 ADC cycles/104us)
		// do nothing
	}
	uint16_t val = ADCL;									// Read ADCL ...
	val |= (ADCH<<8);										// and add ADCH left shifted by 8 bit
	return val;
}

int readX() {
	/*
	Reading X             | Reading Y
	------------------------------------------
	ADC7  x_le    HIGH    | ADC7  x_le    ADC
	ADC6  x_ri    LOW     | ADC6  x_ri    Hi Z
	ADC5  y_up    ADC     | ADC5  y_up    HIGH
	ADC4  y_lo    Hi Z    | ADC4  y_lo    LOW
	*/

	setTOCM(false);
	// Setting up pin modes
	DDRF = 0xC0;											// Setting PF7 and PF6 as output
	PORTF = 0x80;											// Setting PF7 HIGH
	ADMUX = 0x45;											// Making sure ADC5 is connected to ADC
	// Enable ADC and start conversion
	// ADCSRA |= (1<<ADSC) | (1<<ADEN);
	return getADC();
}

int readY() {
	/*
	Reading X             | Reading Y
	------------------------------------------
	ADC7  x_le    HIGH    | ADC7  x_le    ADC
	ADC6  x_ri    LOW     | ADC6  x_ri    Hi Z
	ADC5  y_up    ADC     | ADC5  y_up    HIGH
	ADC4  y_lo    Hi Z    | ADC4  y_lo    LOW
	*/

	setTOCM(false);
	// Setting up pin modes
	DDRF = 0x30;											// Setting PF5 and PF4 as output
	PORTF = 0x20;											// Setting PF5 HIGH
	ADMUX = 0x47;											// Connecting ADC7 to ADC
	// Enable ADC and start conversion
	// ADCSRA |= (1<<ADSC) | (1<<ADEN);
	return getADC();
}

// float doSomeAveraging(float vals[]) {
//   float avrg = 0;
//   for (int i = 0; i < OVERSAMPLING; i++) {
//     avrg += vals[i];
//   }
//   return avrg / OVERSAMPLING;
// }

float doSomeMedianFiltering(int *p, int n, int clamp) {
  /*
  Implementation of median filter. All values in the range
  
    <value at center of array>-clamp < value < <value at center of array>+clamp

  will be averaged to the final value.
  */
  int m = 0;
  int sum = 0;
  for (int i = 0; i < n; i++) {
    if ((*p+n/2 < (*p+n/2)+clamp) | (*p+n/2 > (*p+n/2)-clamp)) {
      sum += *p;
      m++;
    }
    *p++;
  }
  return sum / (float)m;
}