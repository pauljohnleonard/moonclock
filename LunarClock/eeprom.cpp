#include <EEPROM.h>
#include "myio.h"
#include "tilt.h"
#include "phase.h"
#include "ui.h"
// set microseconds for 70deg and -70deg points. Overridden by EEPROM values
int tilt_servoDownLimit = SERVO_MIN + 200;
int tilt_servoUpLimit = SERVO_MAX - 200;

// phase offset for moon in degs.  Overridden by EEPROM values
int phase_Offset = 0;
int sensor1_max = 908;
int sensor1_min = 115;
int sensor2_max = 916;
int sensor2_min = 111;

static const int MAGIC=0;


static int myaddr(int i) {
  return 2*i; 
}

static int myget(int i,int &val) {
    EEPROM.get(myaddr(i),val);  
}


static int myput(int i,int val) {
    EEPROM.put(myaddr(i),val);  
}


static void eeprom_default() {
  // set microseconds for 70deg and -70deg points
  tilt_servoDownLimit = SERVO_MIN + 200;
  tilt_servoUpLimit = SERVO_MAX - 200;

  // phase offset for moon in degs
  phase_Offset = 0;
  sensor1_max = 908;
  sensor1_min = 115;
  sensor2_max = 916;
  sensor2_min = 111;
}


void eeprom_read() {

  int i=1;

  int mag;
  
  myget(0,mag);
  
  if (mag != MAGIC) {
    ui_fatal(F(" Wrong MAGIC in EEPROM please calibrate and save "));
    eeprom_default();
    return;  
  }
  
  myget(i++, tilt_servoDownLimit);
  myget(i++, tilt_servoUpLimit);
  myget(i++, phase_Offset);
  myget(i++, sensor1_min);
  myget(i++, sensor1_max);
  myget(i++, sensor2_min);
  myget(i++, sensor2_max);

}

void eeprom_write() {

  int i=1;
  
  myput(0,MAGIC);

  myput(i++, tilt_servoDownLimit);
  myput(i++, tilt_servoUpLimit);
  myput(i++, phase_Offset);
  myput(i++, sensor1_min);
  myput(i++, sensor1_max);
  myput(i++, sensor2_min);
  myput(i++, sensor2_max);

}

