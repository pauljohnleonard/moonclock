// Lunar Clock Arduino code
// P.J.Loeanrd 2016
// https://github.com/pauljohnleonard/moonclock
//---------------------------------------------------

#include "phase.h"
#include "moon.h"
#include "tilt.h"
#include "ui.h"
#include "mytime.h"
#include "data.h"
#include "myio.h"
#include "eeprom.h"


void update_moon_state() {

  MyTime t=MyTime();
  data_setIndexAt(t.getJH());
  moon_updateState(t);

}


void loop() {

  // poll for user interaction
  ui_poll();
  
  update_moon_state();                           // use the RTC to update the moon model.

  // If the system is running set tilt and phase
  if (RUNNING) {                                // don't try to use the phase system if not RUNNING. 
    tilt_set(moon_tilt, 1000);                  // limit servo time to 200 millis (Note return straight away);
    if (!tilt_running()) phase_set(moon_phase); // Don't try to set phase if the tilt servo is active. 
  } else {
    tilt_set(ui_tilt, 500);                     // SYstem not running so set tilt the user set value.
  }
}


void setup() {

  Serial.begin(9600);
 
  serial = & Serial;
  
  // restore sved state from EEPROM
  eeprom_read();

  // initialize phase table.
  data_setup();

  delay(500);

  tilt_setup();
  phase_setup();
  update_moon_state();

  ui_welcome();
  ui_printError();
  myprintf("\n>");
  
}



