#include "ui.h"
#include "tilt.h"
#include "phase.h"
#include "tilt.h"
#include "global.h"
#include "mytime.h"
#include "data.h"
#include "myio.h"
#include "eeprom.h"
#include "moon.h"
#include "pins.h"


bool RUNNING = true;  // System is up and running
bool BREAK = false;   // Something wants to halt the system (or has det RUNNING false) so report it.

// These values are used when the system is not running so we can test.
int ui_tilt  = 0;
int ui_phase = 0;

#define FLASH_MILLIS 500

// Record the last error message here. Rest using "!"
const __FlashStringHelper  *error_mess = NULL;


//  Max line length for UI input
#define MAX_CHAR 80


void ui_setup() {
  // set the digital pin as output:
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(LED_Y_PIN, OUTPUT);
  digitalWrite(LED_R_PIN, LOW);
  digitalWrite(LED_G_PIN, LOW);
  digitalWrite(LED_B_PIN, LOW);
  digitalWrite(LED_Y_PIN, LOW);


}



void ui_display_led() {

  bool flash=(millis()/FLASH_MILLIS)%2;
  
  int R, B, G , Y ;
  R = B = G = Y = HIGH;

  if ( (error_mess != NULL) || PHASE_BROKEN ) R = LOW;

  if (!RUNNING) {
    Y = LOW;
  } else {
    if (flash) G = LOW;
  }

  if (tilt_running()) {
    B = LOW;
  }

  
  digitalWrite(LED_R_PIN, R);
  digitalWrite(LED_G_PIN, G);
  digitalWrite(LED_B_PIN, B);
  digitalWrite(LED_Y_PIN, Y);


}



bool ui_printError() {
  if (error_mess != NULL) {
    myprintf(F("\n*!*"
               "\n*!* "));
    myprintf(error_mess);
    myprintln();
    myprintf(F("*!* \n"));
    return true;
  }
  return false;
}

void ui_fatal(const __FlashStringHelper  *mess) {
  error_mess = mess;
  RUNNING = false;
  BREAK = true;
}

// Is there a error message
bool ui_hasError() {
  return ( error_mess != NULL );
}


void ui_prompt() {
  MyTime t = MyTime();

  ui_printError();

  t.printJD(F("\n"));
  myprintln();
  if (!RUNNING)           myprintf(F("*!*  SYSTEM IS NOT RUNNING           *!*\n"));
  if (PHASE_BROKEN)       myprintf(F("*!*  PHASE SYSTEM BROKEN FLAG IS SET *!*\n"));
  if (mytime_speed != 0 ) myprintf(F("*!*  CLOCK SPEED SET %4d mins/sec   *!*\n"),mytime_speed);
  myprintf(">");
}


void _ui_printState() {


  MyTime t = MyTime();
  moon_updateState(t);


  float phaseAng = phase_read();

  myprintf(F("** PHASE DRIVE SYSTEM\n"));
  myprintf(F("    phaseOffset : %d\n"), phase_Offset);
  myprintf(F(" Sensors range  : %d->%d  %d->%d\n"), sensor1_min, sensor1_max, sensor2_min, sensor2_max);
  myprintf(F(" Sensor state   : ang=%s v1=%d v2=%d \n"), myf2str(phaseAng), phase_sense1, phase_sense2);

  myprintf(F("** TILT DRIVE SYSTEM\n"));
  myprintf(F(" servo state    : %d->%d->%d\n"), tilt_servoDownLimit, servo_pos, tilt_servoUpLimit);


  myprintf(F("** LUNAR \n"));
  myprintf(F("       JH index : %d\n"), data_index());

  MyTime tp = MyTime(data_jh_prev(), 0, 0);

  MyTime tn = MyTime(data_jh_next(), 0, 0);

  if (data_isWaxing()) {
    tp.print(F("  Prev New:"));
    tn.print(F("\n Next Full:"));
    myprintln();
  } else {
    tp.print(F(" Prev Full: "));
    tn.print(F("\n  Next New: "));
    myprintln();
  }

  myprintf(F(" moon tilt      : %s \n"), myf2str(moon_tilt));
  myprintf(F(" moon alt       : %s \n"), myf2str(moon_alt));
  myprintf(F(" moon phase     : %s \n"), myf2str(moon_phase));
  myprintf(F(" season ang     : %s \n"), myf2str(season_ang));


  ui_printError();

}

void ui_welcome() {

  myprintf(F("\n\n"
             "********* Lunar Clock **************************\n"));
  myprintf(F("*    P.J.Leonard and G.Burgess %s     *\n"), __DATE__);
  myprintf(F("* https://github.com/pauljohnleonard/moonclock *\n"));
  myprintf(F("************************************************\n\n"));

  data_welcome();
  _ui_printState();

  myprintf( F("** SETUP \n"));
  myprintf( F("Set time (GMT) : D year month day hour [minute [sec]] \n"));

  myprintf( F("Phase offset   : O degree\n"));
  myprintf( F("Servo range(us): t min max\n"));
  myprintf( F("PhaseSensor Cal: C\n"));
  myprintf( F("Write to EEPROM: W\n"));


  myprintf( F("** MISC \n"));

  myprintf( F("Full/New Moons : M n \n"));
  myprintf( F("Lunar table    : L [N incHour year month day hour minute sec] \n"));
  myprintf( F("Halt system    : H \n"));
  myprintf( F("Manual Tilt    : T deg \n"));
  myprintf( F("Manual Phase   : P deg \n"));
  myprintf( F("Reset and Run  : R \n"));
  myprintf( F("Speed (demo)   : S minsPerSec \n"));
  myprintf( F("Clear error    : ! \n"));
  myprintf( F("Help(this)     : ? \n"));

}

static int in_range(int min1, int max1, int val) {
  return max(min(max1, val), min1);
}

// Process a line.
static void ui_command(char *cmd) {


  char *tok;

  MyTime tm = MyTime();
  int n = 1, incHr = 1;

  int  phaseOff, d, u, tmp;

  myflush();
  myprintln();


  switch (cmd[0]) {

    case 'M':
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        n = 10;
      } else {
        n = in_range(1, 200, atoi(tok));
      }
      moon_halfTable(n);
      break;

    case 'C':
      phase_calibrate();
      break;

    case '!':
      error_mess   =  NULL;
      PHASE_BROKEN = false;
      BREAK = false;
      break;

    case 'H':
      phase_halt();

      RUNNING = false;
      break;

    case 'T':   // tilt override.
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      ui_tilt = in_range(-80, 80, atoi(tok));
      if (RUNNING) myprintf(F("*!* Halt system to take effect \n"));
      break;

    case 'D':
      {
        MyTime t1(&cmd[1]);
        t1.writeRTC();
        delay(1500);
        MyTime t2 = MyTime();
        t2.print(F("\n check me : "));
        myprintln();
      }
      break;

    case 'P':   // phase
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      ui_phase = in_range(-180, 180, atoi(tok));
      if (RUNNING) myprintf(F("*!* Halt system to take effect \n"));
      else phase_set(ui_phase);
      break;

    case 'O':   // phase offset
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      phaseOff = in_range(-360, 360, atoi(tok));
      phase_setOffset(phaseOff);
      break;


    case 't':
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      d = in_range(SERVO_MIN, SERVO_MID, atoi(tok));
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      u = in_range(SERVO_MID, SERVO_MAX, atoi(tok));
      tilt_setLimits(d, u);
      break;

    case 'S':
      tok = strtok(cmd, " :");
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      mytime_setSpeed(in_range(-100, 100, atoi(tok)));
      break;

    case 'W':
      eeprom_write();
      break;

    case 'R':
      BREAK = false;
      RUNNING = true;
      error_mess = NULL;
      mytime_setSpeed(0);
      break;

    case '?':
      ui_welcome();
      break;

    case 'L':
      {
        int Minute = tm.getMinute();
        int Second = tm.getSecond();
        int Year = tm.getYear();
        int Month = tm.getMonth();
        int Day = tm.getDay();
        int Hour = tm.getHour();

        tok = strtok(cmd, " :");
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        n = in_range(1, 200, atoi(tok));

        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        incHr = in_range(1, 366 * 24, atoi(tok));

        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Year = in_range(2016, 2200, atoi(tok));
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Month = in_range(1, 12, atoi(tok));
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Day = in_range(1, mytime_daysInMonth(Month, Year), atoi(tok));
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Hour = in_range(0, 23, atoi(tok));
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Minute = in_range(0, 59, atoi(tok));
        tok = strtok(NULL, " ");
        if (tok == NULL) goto OK2;
        Second = in_range(0, 59, atoi(tok));

OK2:
        MyTime tt = MyTime(Year, Month, Day, Hour, Minute, Second);
        moon_printTable(tt, n, incHr);
      }

      break;

  }

  ui_prompt();
  return;
INVALID:
  myprintf(F("\n Command error \n>"), cmd);
  ui_prompt();
  return;
}


// User hits any character to break system.

bool ui_poll_break() {
  ui_display_led();
  if (BREAK) return true;
  char c = myraw(false);
  if (c == 0) return false;
  ui_fatal(F(" User BREAK. "));
  return true;
}


// Poll for input.
// Build up a string.
// When a CR OR NL is recieved process the command

static void ui_poll2() {

  static char input_string[MAX_CHAR];        // a string to hold incoming data
  static int str_cnt = 0;

  while (serial->available()) {
    // if (str_cnt == 0) Serial.println();
    char inChar = (char)serial->read();
    serial->write(inChar);

    if (inChar == 8) {
      if (str_cnt > 0) str_cnt--;
      serial->write(' ');
      serial->write(8);
      return;
    }

    input_string[str_cnt] = inChar;

    if (inChar == 13 || inChar == 10 ) {
      input_string[str_cnt] = 0;
      ui_command(input_string);
      str_cnt = 0;
    } else {
      if (str_cnt < MAX_CHAR - 1) str_cnt++;
    }
  }
}


void ui_poll() {

  //  if (BREAK) {
  //    ui_printError();
  //    BREAK = false;
  //  }

  ui_poll2();
}

