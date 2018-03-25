// Lunar Clock Arduino code
// P.J.Loeanrd 2016
// March 2018  switch the tilt sign for new servo
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
#include "global.h"
#include "servo.h"



void update_moon_state() {

  MyTime t;
  data_setIndexAt(t.getJH());
  moon_updateState(t);

}



// This is called repeatedly by the system.
void loop() {

  ui_display_led();                              // We must have flashing LEDS!!!!!
  ui_poll();                                     // poll for user interaction

  if (!servo_attached()) update_moon_state();      // use the RTC to update the moon model.

  // If the system is running set tilt and phase
  if (RUNNING) {                                // don't try to use the phase system if not RUNNING.
    tilt_set(moon_tilt, SERVO_MAX_ATTACH_TIME);     // limit servo time tmillis (Note return straight away);
    if (!servo_attached()) phase_set(moon_phase); // Don't try to set phase if the tilt servo is active.
  } else {
    tilt_set(ui_tilt, SERVO_MAX_ATTACH_TIME);                    // System not running so set tilt the user set value.
  }

  phase_halt();                                 // phase should only run blocking in closed loop.
}


void setup() {

  Serial.begin(9600);
  serial = & Serial;                            // We could use a different Stream if we wanted.

  ui_setup();
  eeprom_read();                                // restore sved state from EEPROM

  data_setup();                                 // initialize phase table.

  delay(500);

  tilt_setup();
  phase_setup();

  update_moon_state();
  ui_welcome();
  ui_printError();                             // Hopefully nothing
  myprintf("\n>");

}

