#include "led.h"
#include "oled.h"
#include "emoji.h"
#include "head.h"
#include "sensor.h"

void setup() {
  Serial.begin(115200);
  setup_FFat();
  setup_audio();
  setup_oled();
  setup_head();
  setup_sensor();
  emoji_init();
}

void loop() {
  handle_gesture();
  test_audio();
}