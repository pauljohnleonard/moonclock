#include <Servo.h>
#include "pins.h"
#include "servo.h"
#include "myio.h"


Servo myservo;


void servo_attach() {
  myprintf(" ATTACHING \n");  
  myservo.attach(SERVO_PIN);
}

void servo_detach() {
  myprintf(" DEATTACHING \n");  
  myservo.detach();
}


bool servo_attached() {
  return myservo.attached();
}

void servo_writeMs(int mS) {
   myprintf(" WRITE \n");     
   myservo.writeMicroseconds(mS);
}



static int servo_readV() {
  return analogRead(SERVO_VOLT_PIN);
}
