#ifndef Head_h
#define Head_h

#include <Servo.h>
#include "common.h"

// Servo
#define X_PIN 12
#define Y_PIN 13
#define STEP 1
#define SERVO_DELAY 10

#define X_OFFSET 25
#define Y_OFFSET 50
extern int X_CENTER;
extern int Y_CENTER;
extern int X_MIN;
extern int X_MAX;
extern int Y_MIN;
extern int Y_MAX;

extern Servo servo_x;
extern Servo servo_y;

// Functions
void servo_init();  
void setup_head();  
void head_move(int x_offset = 0, int y_offset = 0, int servo_delay = SERVO_DELAY);  
void head_center(int servo_delay = SERVO_DELAY);  
void head_right(int offset = 0);  
void head_left(int offset = 0);  
void head_down(int offset = 0);  
void head_up(int offset = 0);  
void head_nod(int servo_delay = 1);  
void head_shake(int servo_delay = 1);  
void head_roll_left(int servo_delay = SERVO_DELAY);  
void head_roll_right(int servo_delay = SERVO_DELAY);  
void random_act();  

#endif