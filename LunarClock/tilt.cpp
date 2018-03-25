
#include "tilt.h"
#include "pins.h"
#include "myio.h"
#include "eeprom.h"
#include "global.h"
#include "ui.h"
#include "servo.h"


bool tilt_low_volt = false;
int servo_pos = 0;
int servo_retry_count = 0;
bool TILT_BROKEN = false;

void tilt_setup() {

  // nothing to do here
}

// Set the servo limits.
void tilt_setLimits(int d, int u) {
  tilt_servoUpLimit = min(SERVO_MAX, max(u, SERVO_MID));
  tilt_servoDownLimit = min(SERVO_MID, max(d, SERVO_MIN));
}



// This must be called as manty times as possible whilst servo is active.
//  0 degrees is center.
void tilt_set(float ang, int maxOnTime) {


  static int servo_pos_last = 0;
  static long servo_on_time = 0;
  static bool servo_retry_flag = false;

  static int DT = 50;
  static long errMax = 0;
  static long tNext = 0;

  float fact1 = (TILT_ANG_UP - ang*tilt_parity) / (TILT_ANG_UP - TILT_ANG_DOWN);
  fact1 = min(1.0, max(0.0, fact1));
  servo_pos = tilt_servoDownLimit * fact1 + tilt_servoUpLimit * (1.0 - fact1);


  // check for low voltage (servo is really active).
  int servoV = servo_readV();


  long tNow = millis();

  if (servoV < LOW_VOLT ) {
    myprintf(F(" Low servo voltage %d \n"), servoV);
    if (servo_attached()) {
      servo_detach();
      servo_retry_flag = true;
      servo_retry_count++;

      if (servo_retry_count > SERVO_MAX_TRY) {        // Servo is probably stuck abort
        myprintf(F(" SERVO DETACHING(RETRY LIMIT EXCEEDED)   %d   %ld\n>" ), servo_retry_count, errMax);
        ui_fatal(F("SERVO RETRY LIMIT REACHED"));
        servo_retry_flag = false;
        TILT_BROKEN = true;
      } else {
        myprintf(F(" SERVO DETACHING(WILL RETRY)   %d   %ld\n>" ), servo_retry_count, errMax);
      }
    }

    tNext = tNow + SERVO_RETRY_DELAY;

    return;
  }



  long err = tNow - tNext;

  if (err < 0) return;  //  not reached tNext yet


  // start up servo if need be;
  if (!(servo_pos == servo_pos_last) || servo_retry_flag ) {   // start the servo running if postion has changed or retry has been requested
    if (!servo_retry_flag) {
      myprintf(F("\n Setting SERVO @ %d.%d(deg)  %d(us)  .. please wait .. "), (int)floor(ang), (int)(10 * (ang - floor(ang))), servo_pos);
    } else {
      myprintf(F("\n Retry   SERVO @ %d.%d(deg)  %d(us)  .. please wait .. "), (int)floor(ang), (int)(10 * (ang - floor(ang))), servo_pos);
    }
    servo_attach();
  
    servo_writeMs(servo_pos);
    servo_pos_last = servo_pos;
    servo_on_time = tNow;
    servo_retry_flag = false;
    tNext = tNow + DT;
    errMax = 0;
  }


  if (err > errMax) {
    errMax = err;
  }

  // if servo is not running we have nothing to do.
  if (!servo_attached()) {
    return;
  }


  //  myprintf(F("ServoV= %d \n"),servoV);


  if (((tNow - servo_on_time) > maxOnTime)) {
    myprintf(F(" SERVO DETACHING  (OK)   %d   %ld\n>" ), servo_retry_count, errMax);
    servo_retry_flag = false;
    servo_retry_count = 0;
    servo_detach();
  }

  tNext += DT;
  return;
}






