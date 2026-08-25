#include "emoji.h"
#include "head.h"
#include "connect.h"
#include "act.h"
#include "animation.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  Serial.println("Initializing...");

  setup_FFat();
  setup_BLE();
  setup_emoji();
  setup_head();

  last_time = millis();
  Serial.println("Robot initialization Done.");
}

void loop() {
  handle_cmd();

  if (enable_act && millis() - last_time > random(6, 10) * 1000) {
    random_act();
    last_time = millis();
  }

  delay(10);
}
