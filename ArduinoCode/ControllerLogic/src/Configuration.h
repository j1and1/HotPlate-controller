#pragma once

#include <Arduino.h>

// pin definitions
#define BTN1 4 // up
#define BTN2 5 // select
#define BTN3 6 // down
#define SENS1 A0 // thermal resistor pin
#define OUT1 12 // Solid state relay pin

#define SENS1_TYPE 1 // thermistor type

// thermal runaway timeout - the time after the heater disables if there are no temperature updates
#define THERMAL_RUNAWAY_TIMEOUT 60000 // milliseconds
#define THERMAL_RUNAWAY_DEGREE_DIFF 3
// temperature PID controller values (just stole these from my 3D printer configuration)
#define P 13.12
#define I 1.522
#define D 65.83
// maximum temperature the heatplate can reach (according to spec sheet)
#define MAX_TEMP 260

// reflow curve I found on the interwebs
const byte CURVE_SIZE = 6;
const double CURVE_TIME[] = { 0.0, 90.0, 180.0, 240.0, 270.0, 360.0};
const double CURVE_TEMP[] = { 0.0, 150.0, 150.0, 240.0, 240.0, 0};
