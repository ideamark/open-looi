#ifndef Sensor_h
#define Sensor_h

#include <RevEng_PAJ7620.h>
#include "emoji.h"
#include "head.h"

extern RevEng_PAJ7620 sensor;

void setup_sensor();
void handle_gesture();

#endif