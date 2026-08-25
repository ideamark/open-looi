#include "sensor.h"


RevEng_PAJ7620 sensor = RevEng_PAJ7620();

void setup_sensor() {
  if (!sensor.begin()) {
    log_error("Gesture Sensor Initiate Failed.");
    // Skip
  } else {
    log_info("Gesture Sensor is Ready.");
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
      log_info("Gesture: Forward");
      eye_surprise();
      eye_blink();
      start_chat = true;
      break;
    case GES_BACKWARD:
      log_info("Gesture: Backward");
      eye_blink();
      break;
    case GES_UP:
      log_info("Gesture: Left");
      head_left(20);
      delay(1000);
      break;
    case GES_DOWN:
      log_info("Gesture: Right");
      head_right(20);
      delay(1000);
      break;
    case GES_RIGHT:
      log_info("Gesture: Up");
      head_up(20);
      break;
    case GES_LEFT:
      log_info("Gesture: Down");
      head_down(20);
      break;
    case GES_CLOCKWISE:
      log_info("Gesture: Clockwise");
      eye_happy();
      head_center();
      eye_blink();
      break;
    case GES_ANTICLOCKWISE:
      log_info("Gesture: Anticlockwise");
      play_animation(random(0, 42));
      break;
    case GES_WAVE:
      log_info("Gesture: Wave");
      eye_happy();
      head_shake(3);
      eye_blink();
      break;
    case GES_NONE:
      break;
  }
}
