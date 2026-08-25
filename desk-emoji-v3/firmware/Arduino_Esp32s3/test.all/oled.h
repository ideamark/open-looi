#ifndef OLED_H
#define OLED_H

#include <Adafruit_SSD1306.h>
#include "common.h"

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

extern Adafruit_SSD1306 oled;

// Functions
void setup_oled();
void oled_print(String text, int delay_time = 100);
void oled_println(String text, int delay_time = 100);

#endif
