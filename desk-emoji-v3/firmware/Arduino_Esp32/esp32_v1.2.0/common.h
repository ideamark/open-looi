#ifndef Common_h
#define Common_h

#include <Arduino.h>
#include <Wire.h>
#include <FFat.h>

extern unsigned long last_time;
extern bool enable_act;

void setup_FFat();

#endif