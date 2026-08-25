#ifndef Emoji_h
#define Emoji_h

#include "common.h"

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
void emoji_init(); 

#endif