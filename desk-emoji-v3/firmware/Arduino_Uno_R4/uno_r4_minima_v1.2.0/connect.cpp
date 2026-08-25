#include "connect.h"

std::queue<String> commandQueue;

void handle_cmd() {
  String cmd = "";

  if (!commandQueue.empty()) {
    cmd = commandQueue.front();
    commandQueue.pop();
    cmd.trim();
  } else if (Serial.available() > 0) {
    cmd = Serial.readStringUntil('\n');
    cmd.trim();
  }

  if (!cmd.isEmpty()) {
    last_time = millis();
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, cmd);

    if (error) {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      return;
    }

    if (doc["actions"].is<JsonArray>()) {
      JsonArray actions = doc["actions"].as<JsonArray>();
      for (JsonVariant action : actions) {
        String actionCmd = action.as<String>();
        executeCommand(actionCmd);
      }
    }

    if (doc["factory"].is<String>()) {
      String factoryCmd = doc["factory"].as<String>();
      executeFactoryCommand(factoryCmd);
    }
  }
}

void executeCommand(String cmd) {
  if (cmd == "eye_blink") {
    Serial.println("eye_blink");
    eye_blink();
    delay(100);
  } else if (cmd == "eye_sleep") {
    Serial.println("eye_sleep");
    eye_sleep();
    delay(100);
  } else if (cmd == "eye_wakeup") {
    Serial.println("eye_wakeup");
    eye_wakeup();
    delay(100);
  } else if (cmd == "eye_happy") {
    Serial.println("eye_happy");
    eye_happy();
    delay(100);
  } else if (cmd == "eye_sad") {
    Serial.println("eye_sad");
    eye_sad();
    delay(100);
  } else if (cmd == "eye_anger") {
    Serial.println("eye_anger");
    eye_anger();
    delay(100);
  } else if (cmd == "eye_surprise") {
    Serial.println("eye_surprise");
    eye_surprise();
    delay(100);
  } else if (cmd == "eye_right") {
    Serial.println("eye_right");
    eye_right();
    delay(100);
  } else if (cmd == "eye_left") {
    Serial.println("eye_left");
    eye_left();
    delay(100);
  } else if (cmd == "head_left") {
    Serial.println("head_left");
    head_left(20);
    delay(100);
  } else if (cmd == "head_right") {
    Serial.println("head_right");
    head_right(20);
    delay(100);
  } else if (cmd == "head_up") {
    Serial.println("head_up");
    head_up(20);
    delay(100);
  } else if (cmd == "head_down") {
    Serial.println("head_down");
    head_down(20);
    delay(100);
  } else if (cmd == "head_center") {
    Serial.println("head_center");
    head_center();
    delay(100);
  } else if (cmd == "head_nod") {
    Serial.println("head_nod");
    head_nod(3);
    delay(100);
  } else if (cmd == "head_shake") {
    Serial.println("head_shake");
    head_shake(3);
    delay(100);
  } else if (cmd == "head_roll_left") { 
    Serial.println("head_roll_left");
    head_roll_left(20);
    delay(100);
  } else if (cmd == "head_roll_right") {  
    Serial.println("head_roll_right");
    head_roll_right(20);
    delay(100);
  } else if (cmd == "delay") {
    Serial.println("delay");
    delay(1000);
  } else {
    Serial.print("Unknown action command: ");
    Serial.println(cmd);
    return;
  }
  Serial.println(cmd);
}

void executeFactoryCommand(String cmd) {
  if (cmd == "reboot" || cmd == "restart") {
    Serial.println("[Factory] Rebooting device...");
    NVIC_SystemReset();
  } else if (cmd == "on") {
    Serial.println("[Factory] Servo Enabled.");
    enable_act = true;
  } else if (cmd == "off") {
    Serial.println("[Factory] Servo Disabled.");
    enable_act = false;
  } else if (cmd.startsWith("head_move")) {
    int firstSpaceIndex = cmd.indexOf(' ');
    if (firstSpaceIndex > 0) {
      int secondSpaceIndex = cmd.indexOf(' ', firstSpaceIndex + 1);
      if (secondSpaceIndex > 0) {
        int thirdSpaceIndex = cmd.indexOf(' ', secondSpaceIndex + 1);
        if (thirdSpaceIndex > 0) {
          String xString = cmd.substring(firstSpaceIndex + 1, secondSpaceIndex);
          String yString = cmd.substring(secondSpaceIndex + 1, thirdSpaceIndex);
          String delayString = cmd.substring(thirdSpaceIndex + 1);
          int x_offset = xString.toInt();
          int y_offset = yString.toInt();
          int servo_delay = delayString.toInt();
          head_move(x_offset, y_offset, servo_delay);
        }
      }
    }
  } else {
    Serial.print("[Factory] Unknown factory command: ");
    Serial.println(cmd);
    return;
  }
  Serial.println(cmd);
}