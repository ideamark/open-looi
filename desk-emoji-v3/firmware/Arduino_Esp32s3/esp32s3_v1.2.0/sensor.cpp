#include "sensor.h"
#include "animation.h"

RevEng_PAJ7620 sensor = RevEng_PAJ7620();

void setup_sensor() {
  if (!sensor.begin()) {
    Serial.println("[Error] Gesture Sensor Initiate Failed.");
    // Skip
  } else {
    Serial.println("Gesture Sensor is Ready.");
  }
}

void handle_gesture() {
  Gesture gesture = sensor.readGesture();

  if (gesture != GES_NONE) {
    servo_x.write(X_CENTER);
    servo_y.write(Y_CENTER);
    last_time = millis();
  }

  switch (gesture) {
    case GES_FORWARD:
      eye_surprise();
      delay(500);
      eye_blink();
      break;
    case GES_BACKWARD:
      eye_blink();
      break;
    case GES_LEFT:
      head_left(20);
      delay(1000);
      break;
    case GES_RIGHT:
      head_right(20);
      delay(1000);
      break;
    case GES_UP:
      head_up(20);
      break;
    case GES_DOWN:
      head_down(20);
      break;
    case GES_CLOCKWISE:
      head_center();
      break;
    case GES_ANTICLOCKWISE:
      play_animation(random(0, 42));
      break;
    case GES_WAVE:
      eye_happy();
      head_shake(3);
      eye_blink();
      break;
    case GES_NONE:
      break;
  }
}
