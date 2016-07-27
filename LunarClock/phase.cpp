#include "global.h"
#include "phase.h"
#include "ang.h"
#include "pins.h"
#include "eeprom.h"   // phasePffset;
#include "myio.h"
#include "ui.h"

#include <math.h>

int phase_sense1;
int phase_sense2;


void phase_test() {

  char buff[10];
  
  long milliprint=millis();
  phase_forward();

  while(!ui_poll_break()) {
    myprintf(F(" ANG= "));
    for (int i=0;i<3;i++) {
      float ang=phase_read();
  
     long milli=millis();    
     if (milli>=milliprint) {
        myprintf(F(" %s %d  %d  | "),myf2str(ang),phase_sense1,phase_sense2);
        milliprint+=200;
     }
    }
    myprintln();
    delay(500);
  }

  phase_halt();
}

void phase_calibrate() {
 
  int max1 = 0, min1 = 1024, max2 = 0, min2 = 1024;
  phase_forward();
  bool calibrtating = true;
  long tNow = millis();

  while ((millis() - tNow) < 100000 ) {
    int v1 = analogRead(SENS1_PIN);

    min1 = min(v1, min1);
    max1 = max(v1, max1);

    int v2 = analogRead(SENS2_PIN);

    min2 = min(v2, min2);
    max2 = max(v2, max2);

  }

  phase_halt();

  sensor1_min = min1;
  sensor1_max = max1;
  sensor2_min = min2;
  sensor2_max = max2;

}


float phase_read() {


  phase_sense1 = analogRead(SENS1_PIN);
  phase_sense2 = analogRead(SENS2_PIN);


  float d1 = (sensor1_max - sensor1_min);
  float d2 = (sensor2_max - sensor2_min);

  float mid1 = (sensor1_max + sensor1_min) / 2.0;
  float mid2 = (sensor2_max + sensor2_min) / 2.0;

  float x1 = (phase_sense1 - mid1) / d1;
  float x2 = (phase_sense2 - mid2) / d2;

  return ang_wrap(phase_Offset + atan2(x1, x2) * 180.0 / M_PI);

}

void phase_back() {
  digitalWrite(MOTOR_POS_PIN, HIGH);
  digitalWrite(MOTOR_NEG_PIN, LOW);
}

#define TOL 1

void phase_set(int phaseNew) {

  if (!RUNNING) return;
  
  int state = 0;

  phaseNew = ang_wrap(phaseNew);

  //Serial.println(phaseNew);

  float phase = phase_read();

  int diff = round(ang_wrap(phaseNew - phase));

  if ( (diff > 2*TOL ) || (diff < -2*TOL) ) {
    myprintf(F("Setting phase to %d \n"), phaseNew);
  } else {
    return;
  }


  float phaseLast=phase+90;

  long t1=millis();
  
  while (true) {

    if (ui_poll_break()) {
      phase_halt();
      RUNNING=false;     // stop running to allow user to tweak
      return;
    }
    
    float phase = phase_read();
    bool doPrint = round(phaseLast) != round(phase);
    
    int diff = round(ang_wrap(phaseNew - phase));

    if (diff > TOL) {

      if (state != 1) {
        phase_forward();
      }
      if (doPrint) myprintf(F("\r> %3d >"), (int)round(phase));
      state = 1;

    } else if (diff < -TOL) {

      if ( state != -1) {
        phase_back();
      }
      if (doPrint) myprintf(F("\r< %3d <"), (int)round(phase));
      state = -1;
    } else {
 
      phase_halt();
      myprintf(F("\r* %3d *  \n>"), (int)round(phase));
      state = 0;
      return;
    }
    
    phaseLast=phase;
    
    if (millis()-t1 > PHASE_SET_TIME_LIMIT) {
      ui_fatal(F("Phase set time limit reached. Check Phase drive system.")); 
      return;
    }
  }
}


void phase_forward() {

  digitalWrite(MOTOR_NEG_PIN, HIGH);
  digitalWrite(MOTOR_POS_PIN, LOW);

}

void phase_halt()
{
  digitalWrite(MOTOR_NEG_PIN, LOW);
  digitalWrite(MOTOR_POS_PIN, LOW);
}


void phase_setup()
{

  pinMode(MOTOR_POS_PIN, OUTPUT);
  pinMode(MOTOR_NEG_PIN, OUTPUT);

  //phase_calibrate();

}

void phase_setOffset(int val) {
  phase_Offset = val;

}
