#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include <RevEng_PAJ7620.h>
#include "common.h"
#include "emoji.h"
#include "head.h"
#include "animation.h"
#include "asr.h"
#include "tts.h"
#include "llm.h"

// Declare external variables used in sensor.cpp
extern RevEng_PAJ7620 sensor;

// Function declarations
void setup_sensor();
void handle_gesture();

#endif