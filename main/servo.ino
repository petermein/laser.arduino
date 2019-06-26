#include <Arduino.h>

void updateServo(){
  // refresh angle
  int angle = myservo.read();

  // change direction when limits
  if (angle >= limits[1] || angle <= limits[0])  speed = -speed;

  myservo.write(angle + speed); 

  // set refresh one time / 2
  refresh = refresh ? false : true;
  if (refresh) SoftwareServo::refresh();
}

