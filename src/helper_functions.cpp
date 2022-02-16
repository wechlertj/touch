#include <Arduino.h>
#include "helper_functions.h"

bool isFingered(void){
	/*
	PF7  x_le    LOW
	PF6  x_ri    Hi Z
	PF5  y_up    Hi Z
	PF4  y_lo    ADC
	*/

	DDRF = 0x80;											// Set PF7 as output, others as input
	PORTF = 0x10;											// Pull-up at ADC4 and set PF7 LOW
	ADMUX = 0x44;											// Connect ADC4

	int check = getADC();
	if (check < 100) {
		return true;
	} else {
		return false;
	}
}

uint16_t getADC(){
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

	// Setting up pin modes
	DDRF = 0xC0;											// Setting PF7 and PF6 as output
	PORTF = 0x80;											// Setting PF7 HIGH
	ADMUX = 0x45;											// Making sure ADC5 is connected to ADC

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

	// Setting up pin modes
	DDRF = 0x30;											// Setting PF5 and PF4 as output
	PORTF = 0x20;											// Setting PF5 HIGH
	ADMUX = 0x47;											// Connecting ADC7 to ADC

	return getADC();
}

float doSomeMedianFiltering(int *p, int n, int clamp) {
  /*
  Implementation of median filter. All values in the range
  
    "value at center of array" - "clamp" < "value" < "value at center of array" + "clamp"

  will be averaged to the final value.
  */
  int m = 0;
  int sum = 0;
  int median_val = *p+n/2;
  for (int i = 0; i < n; i++) {
    if ((*p+i < (median_val)+clamp) & (*p+i > (median_val)-clamp)) {
      sum += *p;
      m++;
    }
  }
  return sum / (float)m; // get avg of sum of clamped values
}

float myMap(float val, float from_low, float from_high, float to_low, float to_high) {
  return (val - from_low) * (to_high - to_low) / (from_high - from_low) + to_low;
}