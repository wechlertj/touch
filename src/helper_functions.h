#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <Arduino.h>
#include "defines.h"

uint16_t getADC();
int readX(void);
int readY(void);
bool isFingered(void);
float doSomeMedianFiltering(int *p, int n, int clamp);
float myMap(float val, float from_low, float from_high, float to_low, float to_high);

#endif