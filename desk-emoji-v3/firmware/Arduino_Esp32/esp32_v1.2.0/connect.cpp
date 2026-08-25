#include "connect.h"

std::queue<String> commandQueue;

void BltCallbacks::onWrite(BLECharacteristic* pCharacteristic) {  
    String value = pCharacteristic->getValue();  
    if (!value.isEmpty()) {  
        String cmd = String(value.c_str());  
        cmd.trim();  
        commandQueue.push(cmd);
    }  
}

void setup_BLE() {
  BLEDevice::init("Desk-Emoji");
  BLEServer* pServer = BLEDevice::createServer();

  BLEService* pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic* pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new BltCallbacks());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Bluetooth is ready, waiting for connection...");
}

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
    eye_blink();
    delay(100);
  } else if (cmd == "eye_happy") {
    eye_happy();
    delay(100);
  } else if (cmd == "eye_sad") {
    eye_sad();
    delay(100);
  } else if (cmd == "eye_anger") {
    eye_anger();
    delay(100);
  } else if (cmd == "eye_surprise") {
    eye_surprise();
    delay(100);
  } else if (cmd == "eye_right") {
    eye_right();
    delay(100);
  } else if (cmd == "eye_left") {
    eye_left();
    delay(100);
  } else if (cmd == "head_left") {
    head_left(20);
    delay(100);
  } else if (cmd == "head_right") {
    head_right(20);
    delay(100);
  } else if (cmd == "head_up") {
    head_up(20);
    delay(100);
  } else if (cmd == "head_down") {
    head_down(20);
    delay(100);
  } else if (cmd == "head_center") {
    head_center();
    delay(100);
  } else if (cmd == "head_nod") {
    head_nod(3);
    delay(100);
  } else if (cmd == "head_shake") {
    head_shake(3);
    delay(100);
  } else if (cmd == "head_roll_left") {
    head_roll_left(20);
    delay(100);
  } else if (cmd == "head_roll_right") {
    head_roll_right(20);
    delay(100);
  } else if (cmd == "delay") {
    delay(1000);

  } else if (cmd == "heart") {
    play_animation(0);
  } else if (cmd == "calendar") {
    play_animation(1);
  } else if (cmd == "face_id") {
    play_animation(2);
  } else if (cmd == "cola") {
    play_animation(3);
  } else if (cmd == "laugh") {
    play_animation(4);
  } else if (cmd == "dumbbell") {
    play_animation(5);
  } else if (cmd == "skateboard") {
    play_animation(6);
  } else if (cmd == "battery") {
    play_animation(7);
  } else if (cmd == "basketball") {
    play_animation(8);
  } else if (cmd == "rugby") {
    play_animation(9);
  } else if (cmd == "alarm") {
    play_animation(10);
  } else if (cmd == "screen") {
    play_animation(11);
  } else if (cmd == "wifi") {
    play_animation(12);
  } else if (cmd == "youtube") {
    play_animation(13);
  } else if (cmd == "tv") {
    play_animation(14);
  } else if (cmd == "movie") {
    play_animation(15);
  } else if (cmd == "cat") {
    play_animation(16);
  } else if (cmd == "write") {
    play_animation(17);
  } else if (cmd == "phone") {
    play_animation(18);
  } else if (cmd == "sunny") {
    play_animation(19);
  } else if (cmd == "cloudy") {
    play_animation(20);
  } else if (cmd == "rainy") {
    play_animation(21);
  } else if (cmd == "windy") {
    play_animation(22);
  } else if (cmd == "snow") {
    play_animation(23);
  } else if (cmd == "beer") {
    play_animation(24);
  } else if (cmd == "walk") {
    play_animation(25);
  } else if (cmd == "shit") {
    play_animation(26);
  } else if (cmd == "cry") {
    play_animation(27);
  } else if (cmd == "puzzled") {
    play_animation(28);
  } else if (cmd == "football") {
    play_animation(29);
  } else if (cmd == "volleyball") {
    play_animation(30);
  } else if (cmd == "badminton") {
    play_animation(31);
  } else if (cmd == "rice") {
    play_animation(32);
  } else if (cmd == "gym") {
    play_animation(33);
  } else if (cmd == "boat") {
    play_animation(34);
  } else if (cmd == "thinking") {
    play_animation(35);
  } else if (cmd == "money") {
    play_animation(36);
  } else if (cmd == "wait") {
    play_animation(37);
  } else if (cmd == "plane") {
    play_animation(38);
  } else if (cmd == "rocket") {
    play_animation(39);
  } else if (cmd == "ok") {
    play_animation(40);
  } else if (cmd == "love") {
    play_animation(41);

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
    ESP.restart();
  } else if (cmd == "on") {
    Serial.println("[Factory] Servo Enabled.");
    enable_act = true;
  } else if (cmd == "off") {
    Serial.println("[Factory] Servo Disabled.");
    enable_act = false;
  } else if (cmd.startsWith("adjust_x")) {
    int firstSpaceIndex = cmd.indexOf(' ');
    if (firstSpaceIndex > 0) {
      String offsetString = cmd.substring(firstSpaceIndex + 1);
      int offset = offsetString.toInt();
      adjust_x_center(offset);
      servo_init();
    }
  } else if (cmd.startsWith("adjust_y")) {
    int firstSpaceIndex = cmd.indexOf(' ');
    if (firstSpaceIndex > 0) {
      String offsetString = cmd.substring(firstSpaceIndex + 1);
      int offset = offsetString.toInt();
      adjust_y_center(offset);
      servo_init();
    }
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