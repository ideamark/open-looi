#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <RevEng_PAJ7620.h>
#include <ESP32Servo.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <queue>
#include <FFat.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Gesture sensor
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

// Servo
#define X_PIN 13
#define Y_PIN 12
#define STEP 1
#define SERVO_DELAY 10
#define X_CENTER_FILE "/X_CENTER.txt"
int X_CENTER = 90;
#define X_OFFSET 25
#define Y_CENTER 90
#define Y_OFFSET 45
int X_MIN = X_CENTER - X_OFFSET;
int X_MAX = X_CENTER + X_OFFSET;
int Y_MIN = Y_CENTER - Y_OFFSET;
int Y_MAX = Y_CENTER + Y_OFFSET;
Servo servo_x;
Servo servo_y;
bool enable_act = true;

#define SERVICE_UUID "4db9a22d-6db4-d9fe-4d93-38e350abdc3c"
#define CHARACTERISTIC_UUID "ff1cdaef-0105-e4fb-7be2-018500c2e927"
static unsigned long last_time = 0;
std::queue<String> commandQueue;

// Adjustable
const int ref_eye_height = 40;
const int ref_eye_width = 40;
const int ref_space_between_eye = 10;
const int ref_corner_radius = 10;

// Current state of the eyes
int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int right_eye_x = 32 + ref_eye_width + ref_space_between_eye;
int left_eye_x = 32;
int left_eye_y = 32;
int right_eye_y = 32;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

// Functions
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (!value.isEmpty()) {
      String cmd = String(value.c_str());
      cmd.trim();
      commandQueue.push(cmd);
    }
  }
};

void setupBLE() {
  BLEDevice::init("Desk-Emoji");
  BLEServer* pServer = BLEDevice::createServer();

  BLEService* pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic* pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Bluetooth is ready, waiting for connection...");
}

void print_error(String msg) {
  Serial.print("[Error] ");
  Serial.println(msg);
}

void adjust_x_center(int offset) {
  File file = FFat.open(X_CENTER_FILE, FILE_WRITE);
  if (file) {
    X_CENTER = X_CENTER + offset;
    file.print(String(X_CENTER));
    file.close();
    Serial.print("Adjust X_CENTER to ");
    Serial.println(X_CENTER);
  } else {
    Serial.print("[Error] Could not write file ");
    Serial.println(X_CENTER_FILE);
  }
}

void update_x_center() {
  File file = FFat.open(X_CENTER_FILE, FILE_READ);
  if (file) {
    String valueStr = file.readString();
    file.close();
    X_CENTER = valueStr.toInt();
    X_MIN = X_CENTER - X_OFFSET;
    X_MAX = X_CENTER + X_OFFSET;
  } else {
    adjust_x_center(0);
  }
  Serial.print("X center angle: ");
  Serial.println(X_CENTER);
}

void draw_eyes(bool update = true) {
  display.clearDisplay();
  //draw from center
  int x = int(left_eye_x - left_eye_width / 2);
  int y = int(left_eye_y - left_eye_height / 2);
  display.fillRoundRect(x, y, left_eye_width, left_eye_height, ref_corner_radius, SSD1306_WHITE);
  x = int(right_eye_x - right_eye_width / 2);
  y = int(right_eye_y - right_eye_height / 2);
  display.fillRoundRect(x, y, right_eye_width, right_eye_height, ref_corner_radius, SSD1306_WHITE);
  if (update) {
    display.display();
  }
}

void eye_center(bool update = true) {
  //move eyes to the center of the display, defined by SCREEN_WIDTH, SCREEN_HEIGHT
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;

  left_eye_x = SCREEN_WIDTH / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = SCREEN_HEIGHT / 2;
  right_eye_x = SCREEN_WIDTH / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = SCREEN_HEIGHT / 2;

  draw_eyes(update);
}

void eye_blink(int speed = 12) {
  draw_eyes();
  for (int i = 0; i < 3; i++) {
    left_eye_height = left_eye_height - speed;
    right_eye_height = right_eye_height - speed;
    draw_eyes();
  }
  for (int i = 0; i < 3; i++) {
    left_eye_height = left_eye_height + speed;
    right_eye_height = right_eye_height + speed;
    draw_eyes();
  }
}

void eye_sleep() {  // DRAWS A LINE TO LOOK LIKE SLEEPING
  left_eye_height = 2;
  right_eye_height = 2;
  draw_eyes(true);
}

void eye_wakeup() {  // WAKE UP THE EYES FROM AN LINE TO ROUND CORNERED SQUARE
  eye_sleep();
  for (int h = 0; h <= ref_eye_height; h += 2) {
    left_eye_height = h;
    right_eye_height = h;
    draw_eyes(true);
  }
}

void eye_happy() {
  eye_center(false);
  //draw inverted triangle over eye lower part
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(
      left_eye_x - left_eye_width / 2 - 1, left_eye_y + offset,
      left_eye_x + left_eye_width / 2 + 1, left_eye_y + 5 + offset,
      left_eye_x - left_eye_width / 2 - 1, left_eye_y + left_eye_height + offset,
      SSD1306_BLACK);
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 1, right_eye_y + offset,
      right_eye_x - left_eye_width / 2 - 1, right_eye_y + 5 + offset,
      right_eye_x + right_eye_width / 2 + 1, right_eye_y + right_eye_height + offset,
      SSD1306_BLACK);
    offset -= 2;
    display.display();
  }
  display.display();
  delay(100);
}

void eye_sad() {
  eye_center(false);
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(
      left_eye_x - left_eye_width / 2 - 5, left_eye_y - offset + 5,
      left_eye_x + left_eye_width / 2 + 5, left_eye_y - 5 - offset,
      left_eye_x - left_eye_width / 2 - 5, left_eye_y - left_eye_height - offset,
      SSD1306_BLACK);
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - offset + 5,
      right_eye_x - left_eye_width / 2 - 5, right_eye_y - 5 - offset,
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - left_eye_height - offset,
      SSD1306_BLACK);
    offset -= 2;
    display.display();
  }
  display.display();
  delay(100);
}

void eye_anger() {
  eye_center(false);
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(
      left_eye_x + left_eye_width / 2 + 5, left_eye_y - offset + 5,
      left_eye_x - left_eye_width / 2 - 5, left_eye_y - 5 - offset,
      left_eye_x + left_eye_width / 2 + 5, left_eye_y - left_eye_height - offset,
      SSD1306_BLACK);
    display.fillTriangle(
      right_eye_x - right_eye_width / 2 - 5, right_eye_y - offset + 5,
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - 5 - offset,
      right_eye_x - right_eye_width / 2 - 5, right_eye_y - left_eye_height - offset,
      SSD1306_BLACK);
    offset -= 2;
    display.display();
  }
  display.display();
  delay(100);
}

void eye_surprise() {
  eye_center(false);
  int initial_width = left_eye_width;
  int initial_height = left_eye_height;
  int min_width = 10;
  int min_height = 10;
  int corner_radius = ref_corner_radius;

  while (initial_width > min_width && initial_height > min_height) {
    display.clearDisplay();
    int x = int(left_eye_x - initial_width / 2);
    int y = int(left_eye_y - initial_height / 2);
    display.fillRoundRect(x, y, initial_width, initial_height, corner_radius, SSD1306_WHITE);

    x = int(right_eye_x - initial_width / 2);
    y = int(right_eye_y - initial_height / 2);
    display.fillRoundRect(x, y, initial_width, initial_height, corner_radius, SSD1306_WHITE);
    display.display();

    initial_width -= 2;
    initial_height -= 2;
    corner_radius = max(corner_radius - 1, 1);
  }
  delay(100);
}

void saccade(int direction_x, int direction_y) {
  //quick movement of the eye, no size change. stay at position after movement, will not move back,  call again with opposite direction
  //direction == -1 :  move left
  //direction == 1 :  move right

  int direction_x_movement_amplitude = 8;
  int direction_y_movement_amplitude = 6;
  int eye_blink_amplitude = 8;

  for (int i = 0; i < 1; i++) {
    left_eye_x += direction_x_movement_amplitude * direction_x;
    right_eye_x += direction_x_movement_amplitude * direction_x;
    left_eye_y += direction_y_movement_amplitude * direction_y;
    right_eye_y += direction_y_movement_amplitude * direction_y;
    right_eye_height -= eye_blink_amplitude;
    left_eye_height -= eye_blink_amplitude;
    draw_eyes();
    delay(1);
  }

  for (int i = 0; i < 1; i++) {
    left_eye_x += direction_x_movement_amplitude * direction_x;
    right_eye_x += direction_x_movement_amplitude * direction_x;
    left_eye_y += direction_y_movement_amplitude * direction_y;
    right_eye_y += direction_y_movement_amplitude * direction_y;
    right_eye_height += eye_blink_amplitude;
    left_eye_height += eye_blink_amplitude;
    draw_eyes();
    delay(1);
  }
}

void move_eye(int direction) {  // MOVES TO RIGHT OR LEFT DEPENDING ON 1 OR -1 INPUT.
  //direction == -1 :  move left
  //direction == 1 :  move right

  int direction_oversize = 1;
  int direction_movement_amplitude = 2;
  int eye_blink_amplitude = 5;

  for (int i = 0; i < 3; i++) {
    left_eye_x += direction_movement_amplitude * direction;
    right_eye_x += direction_movement_amplitude * direction;
    right_eye_height -= eye_blink_amplitude;
    left_eye_height -= eye_blink_amplitude;
    if (direction > 0) {
      right_eye_height += direction_oversize;
      right_eye_width += direction_oversize;
    } else {
      left_eye_height += direction_oversize;
      left_eye_width += direction_oversize;
    }

    draw_eyes();
    delay(1);
  }
  for (int i = 0; i < 3; i++) {
    left_eye_x += direction_movement_amplitude * direction;
    right_eye_x += direction_movement_amplitude * direction;
    right_eye_height += eye_blink_amplitude;
    left_eye_height += eye_blink_amplitude;
    if (direction > 0) {
      right_eye_height += direction_oversize;
      right_eye_width += direction_oversize;
    } else {
      left_eye_height += direction_oversize;
      left_eye_width += direction_oversize;
    }
    draw_eyes();
    delay(1);
  }

  delay(1000);

  for (int i = 0; i < 3; i++) {
    left_eye_x -= direction_movement_amplitude * direction;
    right_eye_x -= direction_movement_amplitude * direction;
    right_eye_height -= eye_blink_amplitude;
    left_eye_height -= eye_blink_amplitude;
    if (direction > 0) {
      right_eye_height -= direction_oversize;
      right_eye_width -= direction_oversize;
    } else {
      left_eye_height -= direction_oversize;
      left_eye_width -= direction_oversize;
    }
    draw_eyes();
    delay(1);
  }
  for (int i = 0; i < 3; i++) {
    left_eye_x -= direction_movement_amplitude * direction;
    right_eye_x -= direction_movement_amplitude * direction;
    right_eye_height += eye_blink_amplitude;
    left_eye_height += eye_blink_amplitude;
    if (direction > 0) {
      right_eye_height -= direction_oversize;
      right_eye_width -= direction_oversize;
    } else {
      left_eye_height -= direction_oversize;
      left_eye_width -= direction_oversize;
    }
    draw_eyes();
    delay(1);
  }
  eye_center();
}

void eye_right() {
  move_eye(1);
}

void eye_left() {
  move_eye(-1);
}

void head_move(int x_offset = 0, int y_offset = 0, int servo_delay = SERVO_DELAY) {
  int x_angle = servo_x.read();
  int y_angle = servo_y.read();
  int to_x_angle = constrain(x_angle + x_offset, X_MIN, X_MAX);
  int to_y_angle = constrain(y_angle + y_offset, Y_MIN, Y_MAX);

  while (x_angle != to_x_angle || y_angle != to_y_angle) {
    if (x_angle != to_x_angle) {
      x_angle += (to_x_angle > x_angle ? STEP : -STEP);
      servo_x.write(x_angle);
    }
    if (y_angle != to_y_angle) {
      y_angle += (to_y_angle > y_angle ? STEP : -STEP);
      servo_y.write(y_angle);
    }
    delay(servo_delay);
  }
}

void head_center(int servo_delay = SERVO_DELAY) {
  int x_angle = servo_x.read();
  int y_angle = servo_y.read();
  head_move(X_CENTER - x_angle, Y_CENTER - y_angle, servo_delay);
}

void head_right(int offset = 0) {
  head_move(offset, 0);
}

void head_left(int offset = 0) {
  head_move(-offset, 0);
}

void head_down(int offset = 0) {
  head_move(0, offset);
}

void head_up(int offset = 0) {
  head_move(0, -offset);
}

void head_nod(int servo_delay = 1) {
  for (int i = 0; i < 2; i++) {
    head_move(0, 20, servo_delay);
    delay(50);
    head_move(0, -20, servo_delay);
    delay(50);
  }
}

void head_shake(int servo_delay = 1) {
  head_move(-10, 0, servo_delay);
  delay(50);
  head_move(20, 0, servo_delay);
  delay(50);
  head_move(-20, 0, servo_delay);
  delay(50);
  head_move(20, 0, servo_delay);
  delay(50);
  head_move(-10, 0, servo_delay);
  delay(50);
}

void head_roll_left(int servo_delay = SERVO_DELAY) {
  head_center();
  head_down(Y_OFFSET / 2 + 5);
  head_move(-X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_center();
}

void head_roll_right(int servo_delay = SERVO_DELAY) {
  head_center();
  head_down(Y_OFFSET / 2 + 5);
  head_move(X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_center();
}

void random_act() {
  int random_num = random(100);
  int x = random(-25, 25);
  int y = random(-20, 45);

  if (random_num < 5) {
    eye_happy();
  } else if (random_num < 30) {
    head_move(x, y, 10);
    if (x > 0) {
      eye_right();
    } else {
      eye_left();
    }
  } else {
    eye_blink();
  }
  head_center();
}

void servo_init() {
  servo_x.attach(X_PIN);
  servo_y.attach(Y_PIN);
  servo_x.write(X_CENTER);
  servo_y.write(Y_CENTER);
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
    case GES_ANTICLOCKWISE:
      head_center();
      delay(500);
      eye_blink();
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

    if (cmd == "eye_blink") {
      eye_blink();
    } else if (cmd == "eye_happy") {
      eye_happy();
    } else if (cmd == "eye_sad") {
      eye_sad();
    } else if (cmd == "eye_anger") {
      eye_anger();
    } else if (cmd == "eye_surprise") {
      eye_surprise();
    } else if (cmd == "eye_right") {
      eye_right();
    } else if (cmd == "eye_left") {
      eye_left();
    } else if (cmd == "head_left") {
      head_left(20);
    } else if (cmd == "head_right") {
      head_right(20);
    } else if (cmd == "head_up") {
      head_up(20);
    } else if (cmd == "head_down") {
      head_down(20);
    } else if (cmd == "head_center") {
      head_center();
    } else if (cmd == "head_nod") {
      head_nod(3);
    } else if (cmd == "head_shake") {
      head_shake(3);
    } else if (cmd == "head_roll_left") {
      head_roll_left(20);
    } else if (cmd == "head_roll_right") {
      head_roll_right(20);
    } else if (cmd == "reboot" || cmd == "restart") {
      ESP.restart();
    } else if (cmd == "on") {
      enable_act = true;
    } else if (cmd == "off") {
      enable_act = false;
    } else if (cmd.startsWith("adjust")) {
      int firstSpaceIndex = cmd.indexOf(' ');
      if (firstSpaceIndex > 0) {
        String xString = cmd.substring(firstSpaceIndex + 1);
        int x_offset = xString.toInt();
        adjust_x_center(x_offset);
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
      Serial.print("Unknown command: ");
    }
    Serial.println(cmd);
  }
}

void setup() {
  bool init_success = true;

  Serial.begin(115200);
  Serial.flush();
  Serial.println("Initializing...");

  // Init bluetooth
  setupBLE();

  // Init gesture sensor
  if (!sensor.begin()) {
    init_success = false;
    print_error("Gesture Sensor Initiate Failed.");
    // Skip
  } else {
    Serial.println("Gesture Sensor is Ready.");
  }

  // Init FATFS
  if (!FFat.begin(true)) {
    init_success = false;
    print_error("FATFS Initiate Failed.");
    while (1);  // Pause
  } else {
    Serial.println("FATFS is Ready.");
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    init_success = false;
    print_error("SSD1306 Initiate Failed.");
    while (1);  // Pause
  } else {
    Serial.println("SSD1306 is Ready.");
    display.clearDisplay();
  }

  // Init movement
  update_x_center();
  eye_sleep();
  delay(2000);
  eye_wakeup();
  eye_center();
  eye_blink();
  servo_init();
  head_center();
  delay(500);
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