
#include "tilt.h"
#include "pins.h"
#include "myio.h"
#include "eeprom.h"
#include "global.h"
#include "ui.h"

#include <Servo.h>

Servo myservo;

bool tilt_low_volt = false;
int servo_pos = 0;
int servo_retry_count = 0;

void tilt_setup()
{  long tNow = millis();

  // tilt_set(0.0,0);
}



// Set the servo limits.
void tilt_setLimits(int d, int u) {
  tilt_servoUpLimit = min(SERVO_MAX, max(u, SERVO_MID));
  tilt_servoDownLimit = min(SERVO_MID, max(d, SERVO_MIN));
}


bool tilt_running() {
  return myservo.attached();
}

// This must be called as manty times as possible whilst servo is active.
//  0 degrees is down
void tilt_set(float ang, int maxOnTime) {


  static int servo_pos_last = 0;
  static long servo_on_time = 0;
  static long servo_drive_time = 0;
  static bool servo_retry_flag=false;
  static long volt_idle=4800;
  static long volt_active=0;

  static int DT=10;
  static long errMax=0;
  static long tNext=0;
  float fact1 = (TILT_ANG_UP - ang) / (TILT_ANG_UP - TILT_ANG_DOWN);
  fact1 = min(1.0, max(0.0, fact1));
  servo_pos = tilt_servoDownLimit * fact1 + tilt_servoUpLimit * (1.0 - fact1);

 
  long tNow = millis();
  

  // start up servo if need be;
  if (!(servo_pos == servo_pos_last) || servo_retry_flag ) {   // start the servo running if postion has changed or retry has been requested
    myprintf(F("\n Setting SERVO @ %d.%d(deg)  %d(us)  %ld .. please wait .. "), (int)floor(ang), (int)(10 * (ang - floor(ang))), servo_pos,volt_idle);
    myservo.attach(SERVO_PIN);
    myservo.writeMicroseconds(servo_pos);
    servo_pos_last = servo_pos;
    servo_on_time = tNow;
    servo_retry_flag = false;
    volt_active=ui_readVcc();
    tNext=tNow+DT;
    errMax=0;
  }

  // if servo is not running we have nothing to do.
  if (!myservo.attached()) {   
    volt_idle=(volt_idle+ui_readVcc())/2;    // keep a record of Vcc under noload with a simple filtering
    return;
  }


  long err=tNow-tNext;
  if (err < 0) return;  //  not reached tNext yet
  
  if (err > errMax) {
    errMax = err;
  }
  
  // check for low voltage (servo is really active).
  long mV = ui_readVcc();

  //myprintf(F("%ld\n"),mV);
      
  if (mV < LOW_VOLT ) {
    servo_drive_time = tNow;
    tilt_low_volt = true;
  } else {
    tilt_low_volt = false;
  }


  if ((tNow - servo_on_time) > maxOnTime) {
    myservo.detach();
    delay(10);
    long servo_spare_time = (tNow - servo_drive_time);
    
    myprintf(F(" SERVO DETACHING  %ld  %d   %ld\n>" ), servo_spare_time,servo_retry_count,errMax);
  
    if (servo_spare_time < SERVO_RETRY_TIME ) {
      servo_retry_count++;
      servo_retry_flag=true;     //  ask to try again
      if (servo_retry_count > SERVO_MAX_TRY) {        // Servo is probably stuck abort
        ui_fatal(F("SERVO RETRY LIMIT REACHED"));
        servo_retry_flag=false;
      }
    } else {
      servo_retry_count = 0;
      servo_retry_flag=false;
    }
  }
  tNext+=DT;
  return;
}






