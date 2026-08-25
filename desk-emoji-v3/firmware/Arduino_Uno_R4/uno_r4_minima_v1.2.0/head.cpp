#include "head.h"

int X_CENTER = 90;
int Y_CENTER = 70;
int X_MIN = X_CENTER - X_OFFSET;
int X_MAX = X_CENTER + X_OFFSET;
int Y_MIN = Y_CENTER - Y_OFFSET;
int Y_MAX = Y_CENTER + Y_OFFSET;

Servo servo_x;
Servo servo_y;

void servo_init() {
  servo_x.attach(X_PIN);
  servo_y.attach(Y_PIN);
  servo_x.write(X_CENTER);
  servo_y.write(Y_CENTER);
}

void setup_head() {
  servo_init();
  head_center();
  delay(500);
}

void head_move(int x_offset, int y_offset, int servo_delay) {
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

void head_center(int servo_delay) {
  int x_angle = servo_x.read();
  int y_angle = servo_y.read();
  head_move(X_CENTER - x_angle, Y_CENTER - y_angle, servo_delay);
}

void head_right(int offset) {
  head_move(offset, 0);
}

void head_left(int offset) {
  head_move(-offset, 0);
}

void head_down(int offset) {
  head_move(0, offset);
}

void head_up(int offset) {
  head_move(0, -offset);
}

void head_nod(int servo_delay) {
  for (int i = 0; i < 2; i++) {
    head_move(0, 40, servo_delay);
    delay(50);
    head_move(0, -40, servo_delay);
    delay(50);
  }
}

void head_shake(int servo_delay) {
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

void head_roll_left(int servo_delay) {
  head_center();
  head_down(Y_OFFSET / 2 + 5);
  head_move(-X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_center();
}

void head_roll_right(int servo_delay) {
  head_center();
  head_down(Y_OFFSET / 2 + 5);
  head_move(X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, -Y_OFFSET / 2, servo_delay);
  head_move(-X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_move(X_OFFSET, Y_OFFSET / 2, servo_delay);
  head_center();
}
