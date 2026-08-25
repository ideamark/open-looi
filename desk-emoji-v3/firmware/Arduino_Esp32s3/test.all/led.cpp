#include "led.h"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup_led() {
  strip.begin();
  strip.show();
  log_info("LED is Ready.");
}

void set_led(uint32_t color, uint8_t brightness) {
  brightness = constrain(brightness, 0, 100);
  strip.setBrightness(map(brightness, 0, 100, 0, 255));
  strip.setPixelColor(0, color);
  strip.show();
}

void blink_led(uint32_t color, uint8_t times, uint8_t brightness) {
  for (uint8_t i = 0; i < times; i++) {
    set_led(color, brightness);
    delay(50);
    set_led(COLOR_BLACK);
    delay(50);
  }
}