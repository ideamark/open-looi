#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Servo
#define X_PIN 19
#define Y_PIN 18
#define STEP 1
#define SERVO_DELAY 10
#define X_CENTER 90
#define X_OFFSET 25
#define Y_CENTER 80
#define Y_OFFSET 50
const int x_min = X_CENTER - X_OFFSET;
const int x_max = X_CENTER + X_OFFSET;
const int y_min = Y_CENTER - Y_OFFSET;
const int y_max = Y_CENTER + Y_OFFSET;
Servo servo_x;
Servo servo_y;

static unsigned long last_time = 0;

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
void draw_eyes(bool update);
void saccade(int direction_x, int direction_y);
void move_eye(int direction);
void eye_center(bool update);
void eye_blink(int speed);
void eye_sleep();
void eye_wakeup();
void eye_happy();
void eye_sad();
void eye_anger();
void eye_surprise();
void eye_right();
void eye_left();
void head_move(int x_offset, int y_offset, int servo_delay);
void head_up(int offset);
void head_down(int offset);
void head_left(int offset);
void head_right(int offset);
void head_center(int servo_delay);
void head_nod(int servo_delay);
void head_shake(int servo_delay);
void head_roll(int servo_delay);
void random_act();
void servo_init();
void servo_detach();

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
      SSD1306_BLACK
    );
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 1, right_eye_y + offset,
      right_eye_x - left_eye_width / 2 - 1, right_eye_y + 5 + offset,
      right_eye_x + right_eye_width / 2 + 1, right_eye_y + right_eye_height + offset,
      SSD1306_BLACK
    );
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
      SSD1306_BLACK
    );
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - offset + 5,
      right_eye_x - left_eye_width / 2 - 5, right_eye_y - 5 - offset,
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - left_eye_height - offset,
      SSD1306_BLACK
    );
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
      SSD1306_BLACK
    );
    display.fillTriangle(
      right_eye_x - right_eye_width / 2 - 5, right_eye_y - offset + 5,
      right_eye_x + right_eye_width / 2 + 5, right_eye_y - 5 - offset,
      right_eye_x - right_eye_width / 2 - 5, right_eye_y - left_eye_height - offset,
      SSD1306_BLACK
    );
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

void head_move(int x_offset=0, int y_offset=0, int servo_delay=SERVO_DELAY) {
  int x_angle = servo_x.read();
  int y_angle = servo_y.read();
  int to_x_angle = constrain(x_angle + x_offset, x_min, x_max);
  int to_y_angle = constrain(y_angle + y_offset, y_min, y_max);

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

void head_right(int offset=0) {
  head_move(offset, 0);
}

void head_left(int offset=0) {
  head_move(-offset, 0);
}

void head_down(int offset=0) {
  head_move(0, offset);
}

void head_up(int offset=0) {
  head_move(0, -offset);
}

void head_center(int servo_delay=SERVO_DELAY) {
  int x_angle = servo_x.read();
  int y_angle = servo_y.read();
  head_move(X_CENTER - x_angle, Y_CENTER - y_angle, servo_delay);
}

void head_nod(int servo_delay=1) {
  for (int i = 0; i < 2; i++) {
    head_move(0, 20, servo_delay);
    delay(50);
    head_move(0, -20, servo_delay);
    delay(50);
  }
}

void head_shake(int servo_delay=1) {
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

void head_roll(int servo_delay=SERVO_DELAY) {
  head_center();
  head_down(Y_OFFSET/2+5);
  head_move(X_OFFSET, -Y_OFFSET/2, servo_delay);
  head_move(-X_OFFSET, -Y_OFFSET/2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET/2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET/2, servo_delay);
  head_move(-X_OFFSET, -Y_OFFSET/2, servo_delay);
  head_move(X_OFFSET, -Y_OFFSET/2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET/2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET/2, servo_delay);
  head_center();
}

void random_act() {
  int random_num = random(100);
  int x = random(-25, 26);
  int y = random(-20, 51);
  if (random_num < 5) {
    eye_happy();
  } else if (random_num < 40) {
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

void servo_detach() {
  servo_x.detach();
  servo_y.detach();
}

void setup() {
  Serial.begin(115200);
  Serial.flush();
  Serial.println("Initializing...");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();

  // Init Servo
  servo_init();
  eye_sleep();
  head_down(100);
  delay(1000);
  eye_wakeup();
  eye_center();
  eye_blink();
  head_center();
  delay(500);
  last_time = millis();
  Serial.println("Robot initialization Done.");
  servo_detach();
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    last_time = millis();
    servo_init();

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
    } else if (cmd == "head_center") {  
      head_center();
    } else if (cmd == "head_nod") {
      head_nod(3);
    } else if (cmd == "head_shake") {
      head_shake(3);
    } else if (cmd == "head_roll") {
      head_roll(30);
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
    servo_detach();
  }
  if (millis() - last_time > random(5, 8) * 1000) {
    servo_init();
    random_act();
    servo_detach();
    last_time = millis();
  }
  delay(10);
}