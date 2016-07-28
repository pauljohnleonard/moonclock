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
#include "bluetooth.h"
#include "myio.h"
#include "eeprom.h"


void update_moon_state() {

  MyTime t;
  mytime_read(t);
  mytime_setJHfromCal(t);
  data_setIndexAt(t.JH);
  moon_updateState(t);
}


void loop() {

  ui_poll();

  update_moon_state();

  if (RUNNING) {   // don't try to use the phase system if not RUNNING.
    phase_set(moon_phase);
    tilt_set(moon_tilt, 1000);      // limit servo time to 200 millis (Note return straight away);
  } else {
    phase_halt();
    tilt_set(ui_tilt, 500);
  }

}


void setup() {

  Serial.begin(9600);

  // but you may get servo jitters during coms if servo is attached.
  //   bluetooth_setup();
  //   serial = &BTSerial;
  
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



