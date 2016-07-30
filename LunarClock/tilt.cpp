
#include "tilt.h"
#include "pins.h"
#include "myio.h"
#include "eeprom.h"
#include "global.h"

#include <Servo.h> 
 
Servo myservo; 

int servo_pos=0;

void tilt_setup() 
{ 
  // tilt_set(0.0,0);
} 


void tilt_setLimits(int d,int u) {
    tilt_PosUp=min(SERVO_MAX,max(u,SERVO_MID));
    tilt_PosDown=min(SERVO_MID,max(d,SERVO_MIN));
}


bool tilt_running() {
  return myservo.attached();
}
//  0 degrees is down
void tilt_set(float ang,int onTime) {

  static int servo_pos_last=0;
  static long servo_on_time =0;
  
  float fact1=(TILT_ANG_UP-ang)/(TILT_ANG_UP-TILT_ANG_DOWN);
  fact1=min(1.0,max(0.0,fact1));
  servo_pos=tilt_PosDown*fact1+tilt_PosUp*(1.0-fact1);   
  
  if (servo_pos == servo_pos_last) {
    if (!myservo.attached()) return;
    if ((millis()-servo_on_time)> onTime) {
      myservo.detach();
      delay(10);
      myprintf(F(" SERVO DETACHING  \n>"));
    }
    return;
  }

  myprintf(F("\n Setting SERVO @ %d.%d(deg)  %d(us)  .. please wait .. "),(int)floor(ang), (int)(10*(ang-floor(ang))),servo_pos);
  myservo.attach(SERVO_PIN);
  myservo.writeMicroseconds(servo_pos);
  servo_pos_last=servo_pos;
  servo_on_time=millis();
  
}






