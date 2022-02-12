#pragma once

// Range of raw ADC values accross the touch screen area
#define XMIN 69.0
#define XMAX 954.0
#define YMIN 102.0
#define YMAX 913.0

// Size of the arrays holding ADC readings
#define OVERSAMPLING 40								// Time to get one xy-pair: ((OVERSAMPLING*2)+2)/ADC_freq
#define CLAMP 5

// Physical dimensions of the sensitive area of the touch screen (in mm)
#define WIDTH 212.2
#define HEIGHT 159.4

// Offsets to place the coordinate origin at the center of the sensitive area
#define VOFFSET -14.0
#define HOFFSET 19.0

// Mouse stuff
#define MOUSE_EN PIND0
#define MOUSE_SPEED 20
#define MOUSE_DELAY 5

// Check touchy
#define FINGERED_UP !(PINF & (1<<PINF5))
#define FINGERED_LO !(PINF & (1<<PINF4))

// Bit position of the LED_L
#define LED_L 7