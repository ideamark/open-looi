#include <Servo.h>


#define X_CENTER 90
#define Y_CENTER 70
Servo servo_x;
Servo servo_y;

void setup()
{
  servo_x.attach(12);
  servo_y.attach(13);
}

void loop()
{
  servo_x.write(X_CENTER);
  servo_y.write(Y_CENTER);
  delay(10);
}
