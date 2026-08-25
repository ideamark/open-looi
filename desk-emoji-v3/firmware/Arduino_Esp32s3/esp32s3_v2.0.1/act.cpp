#include "act.h"

void sequent_act(String actions) {
  if (actions.isEmpty()) {
    return;
  }
  int startIndex = 0;
  int commaIndex;

  while (startIndex < actions.length()) {
    commaIndex = actions.indexOf(',', startIndex);
    String cmd;
  
    if (commaIndex != -1) {
      cmd = actions.substring(startIndex, commaIndex);
      startIndex = commaIndex + 1;
    } else {
      cmd = actions.substring(startIndex);
      startIndex = actions.length();
    }
    
    cmd.trim();
    executeCommand(cmd);
  }
}

void random_act() {
  int random_num = random(100);
  int x = random(-25, 25);
  int y = random(-10, 25);

  if (random_num < 10) {
    eye_happy();
  } else if (random_num > 80) {
    head_move(x, y, 10);
    if (x > 0) {
      eye_right();
    } else {
      eye_left();
    }
    head_center();
  } else {
    eye_blink();
  }
}

void speech_act() {
  int random_num = random(100);
  int x = random(-25, 25);
  int y = random(-10, 25);

  if (random_num > 70) {
    eye_happy();
    if (random_num > 90) {
      head_shake();
    } else {
      head_nod();
    }
    head_center();
    eye_blink();
  } else if (random_num > 40) {
    head_move(x, y, 10);
    if (x > 0) {
      eye_right();
    } else {
      eye_left();
    }
    head_center();
  } else if (random_num > 20) {
    eye_happy();
    if (random_num > 10) {
      head_roll_left();
    } else {
      head_roll_right();
    }
    head_center();
    eye_blink();
  } else {
    eye_blink();
  }
}

void async_sequent_act(String actions) {
  xTaskCreate(
    [](void* parameter) {
      String* actions = (String*)parameter;
      sequent_act(*actions);
      delete actions;
      vTaskDelete(NULL);
    },
    "SequentActTask",
    4096,                        // Stack size
    new String(actions),         // Pass actions as parameter
    1,                           // Priority
    NULL                         // Task handle
  );
}

void async_random_act() {
  xTaskCreate(
    [](void* parameter) {
      randomSeed(esp_random());  // Must seed random number generator
      random_act();
      vTaskDelete(NULL);
    },
    "RandomActTask", 
    4096,                        // Stack size
    NULL,                        // No parameters needed
    2,                           // Priority
    NULL                         // Task handle
  );
}

void async_speech_act() {
  xTaskCreate(
    [](void* parameter) {
      randomSeed(esp_random());  // Must seed random number generator
      speech_act();
      vTaskDelete(NULL);
    },
    "SpeechActTask", 
    4096,                        // Stack size
    NULL,                        // No parameters needed
    2,                           // Priority
    NULL                         // Task handle
  );
}
