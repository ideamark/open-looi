#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>
#include "common.h"

// Define WS2812 pin and LED count
#define LED_PIN 48
#define NUM_LEDS 1

// Common colors - #define for simplicity
#define COLOR_RED      0xFF0000  // Red: Full intensity, Green: 0, Blue: 0
#define COLOR_GREEN    0x00FF00  // Green: Full intensity, Red: 0, Blue: 0
#define COLOR_BLUE     0x0000FF  // Blue: Full intensity, Red: 0, Green: 0
#define COLOR_WHITE    0xFFFFFF  // White: Full Red, Green, Blue
#define COLOR_BLACK    0x000000  // Black (off): Red, Green, Blue all 0
#define COLOR_YELLOW   0xFFFF00  // Yellow: Red + Green, Blue: 0
#define COLOR_CYAN     0x00FFFF  // Cyan: Green + Blue, Red: 0
#define COLOR_MAGENTA  0xFF00FF  // Magenta: Red + Blue, Green: 0
#define COLOR_ORANGE   0xFF8000  // Orange: Red + some Green, Blue: 0
#define COLOR_PURPLE   0x8000FF  // Purple: Blue + some Red, Green: 0

extern Adafruit_NeoPixel strip;

void setup_led();
void set_led(uint32_t color, uint8_t brightness = 100);
void blink_led(uint32_t color = COLOR_BLUE, uint8_t times = 1, uint8_t brightness = 5);

#endif
