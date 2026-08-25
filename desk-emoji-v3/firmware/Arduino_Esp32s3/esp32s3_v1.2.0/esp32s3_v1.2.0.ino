#include "emoji.h"
#include "head.h"
#include "sensor.h"
#include "connect.h"
#include "act.h"
#include "animation.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  Serial.println("Initializing...");

  setup_FFat();
  setup_BLE();
  setup_sensor();
  setup_emoji();
  setup_head();

  last_time = millis();
  Serial.println("Robot initialization Done.");
}

void loop() {
  handle_gesture();
  handle_cmd();

  if (enable_act && millis() - last_time > random(6, 10) * 1000) {
    random_act();
    last_time = millis();
  }

  delay(10);
}

// {"factory": "reboot"}
// {"factory": "adjust_x -10"}   -left, +right
// {"factory": "adjust_y 20"}    -up, +down