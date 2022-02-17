#pragma once

// Range of raw ADC values accross the touch screen area
#define XMIN 68.0
#define XMAX 961.0
#define YMIN 108.0
#define YMAX 917.0

// Size of the arrays holding ADC readings
#ifdef FILTERING
#define OVERSAMPLING 15
#else
#define OVERSAMPLING 1
#endif
#define CLAMP 2

// Physical dimensions of the sensitive area of the touch screen (in mm)
// Datasheet "Guaranteed active area":
// #define WIDTH 212.2
// #define HEIGHT 159.4
// Empirical values below:
#define WIDTH 214.5
#define HEIGHT 161.0

// Offsets to place the coordinate origin at the center of the sensitive area
#define HOFFSET {(XMAX-XMIN)*0.5}
#define VOFFSET {(YMAX-YMIN)*0.5}

// Mouse stuff
#define MOUSE_EN PIND0
#define MOUSE_SPEED 20
#define MOUSE_DELAY 5

// Check touchy
#define FINGERED_UP !(PINF & (1<<PINF5))
#define FINGERED_LO !(PINF & (1<<PINF4))

// Bit position of the LED_L
#define LED_L 7