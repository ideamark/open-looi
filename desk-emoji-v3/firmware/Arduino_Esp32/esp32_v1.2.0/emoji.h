#ifndef Emoji_h
#define Emoji_h

#include <Adafruit_SSD1306.h>
#include "common.h"

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

extern Adafruit_SSD1306 display;

// Functions
void setup_emoji();
void saccade(int direction_x, int direction_y);  
void move_eye(int direction);  
void draw_eyes(bool update = true);  
void eye_center(bool update = true);  
void eye_blink(int speed = 12);  
void eye_sleep();  
void eye_wakeup();  
void eye_happy();  
void eye_sad();  
void eye_anger();  
void eye_surprise();  
void eye_right();  
void eye_left();  

#endif