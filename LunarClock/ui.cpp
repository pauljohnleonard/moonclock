#include "ui.h"
#include "tilt.h"
#include "phase.h"
#include "tilt.h"
#include "global.h"
#include "mytime.h"
#include "data.h"
#include "bluetooth.h"
#include "myio.h"
#include "eeprom.h"
#include "moon.h"


bool RUNNING = true;  // System is up and running
bool BREAK = false;   // Something wants to halt the system (or has det RUNNING false) so report it.
float ui_tilt=0.0;
//  Max line length for UI input
#define MAX_CHAR 60


const __FlashStringHelper  *error_mess = NULL;

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


void ui_printTime(const __FlashStringHelper  *tag, MyTime tm) {

  if (tag != NULL)  myprintf(tag);
  myprintf(F("%02d/%02d/%04d  %2d:%02d:%02d"), tm.Day, tm.Month, tm.Year, tm.Hour, tm.Minute, tm.Second);

}


void ui_prompt() {
  MyTime t;
  mytime_read(t);

  ui_printError();
  
  ui_printTime(F("\n"), t);
  myprintf("\n>");
}


void _ui_printState() {



  MyTime t, tp, tn;
  mytime_read(t);
  moon_updateState(t);

  
  float phaseAng = phase_read();

  myprintf(F("** PHASE DRIVE SYSTEM\n"));
  myprintf(F("    phaseOffset : %d\n"), phase_Offset);
  myprintf(F(" Sensors range  : %d->%d  %d->%d\n"), sensor1_min, sensor1_max, sensor2_min, sensor2_max);
  myprintf(F(" Sensor state   : ang=%s v1=%d v2=%d \n"), myf2str(phaseAng), phase_sense1, phase_sense2);

  myprintf(F("** TILT DRIVE SYSTEM\n"));
  myprintf(F(" servo state    : %d->%d->%d\n"), tilt_PosDown, servo_pos,tilt_PosUp);


  myprintf(F("** LUNAR \n"));
  myprintf(F("       JH index : %d\n"), data_index());

  tp.JH = data_jh_prev();
  mytime_setCalfromJH(tp);

  tn.JH = data_jh_next();
  mytime_setCalfromJH(tn);

  if (data_isWaxing()) {
    ui_printTime(F("  Prev New:"), tp); 
    ui_printTime(F("\n Next Full:"), tn);
    myprintln();
  } else {
    ui_printTime(F(" Prev Full: "), tp);
    ui_printTime(F("\n  Next New: "), tn);
    myprintln();
  }

  myprintf(F(" moon tilt      : %s \n"), myf2str(moon_tilt));
  myprintf(F(" moon alt       : %s \n"), myf2str(moon_alt));
  myprintf(F(" moon phase     : %s \n"), myf2str(moon_phase));
  myprintf(F(" season ang     : %s \n"), myf2str(season_ang));



  if (RUNNING) {
    myprintf(F("\n\n** SYSTEM IS RUNNING\n\n"));
  } else {
    myprintf(F("\n\n** SYSTEM IS NOT RUNNING \n\n"));
  }

  ui_printError();

}

void ui_welcome() {

  myprintf(F("\n\n"
             "********* Lunar Clock *******************\n"));
  myprintf(F("* P.J.Leonard and G.Burgess %s *\n"),__DATE__);
  myprintf(F("*****************************************\n\n"));

  data_welcome();
  _ui_printState();

  myprintf( F("** SETUP \n"));
  myprintf( F("Set time (GMT) : D year month day hour [minute [sec]] \n"));
  
  myprintf( F("Phase offset   : P degree\n"));
  myprintf( F("Tilt 900-2100us: t min max\n"));
  myprintf( F("PhaseSensor Cal: C \n"));
  myprintf( F("Write to EEPROM: W\n"));

  myprintf( F("** MISC \n"));  
  myprintf( F("Lunar table    : L [N incHour year month day hour minute sec] \n"));
  myprintf( F("Halt system    : H \n"));
  myprintf( F("Reset and Run  : R \n"));
  myprintf( F("Clear error    : C \n"));
  myprintf( F("Help(this)     : ?\n"));

}


// Process a line.
static void ui_command(char *cmd) {

  
  char *tok;

  MyTime tm;
  mytime_read(tm);
  int n = 1, incHr=1;

  int  phaseOff, d, u, tmp;

  tok = strtok(cmd, " :");
  
  myprintln();
  
  //  Serial.print("#");
  //  Serial.print(tok);
  //  Serial.println("#");
  //
 
  switch (tok[0]) {

    case 'C':
      error_mess=NULL;
      break;
      
    case 'H':
      RUNNING=false;
      break;

    case 'T':
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      ui_tilt = atoi(tok);
      break;
      
    case 'D':
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      tm.Year = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK;
      tm.Month = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK;
      tm.Day = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK;
      tm.Hour = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK;
      tm.Minute = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK;
      tm.Second = atoi(tok);

OK:
      mytime_write(tm);
      delay(1500);
      mytime_read(tm);
      ui_printTime(F("\n check me : "), tm);
      myprintln();
      break;

    case 'P':
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      phaseOff = atoi(tok);
      phase_setOffset(phaseOff);
      break;

    case 'S':
      phase_test();
      break;

    case 't':
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      d = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto INVALID;
      u = atoi(tok);
      tilt_setLimits(d, u);
      tilt_set(TILT_ANG_DOWN, 2000);
      tilt_set(0, 2000);
      tilt_set(TILT_ANG_UP, 2000);
      break;

    case 'W':
      eeprom_write();
      break;

    case 'R':
      BREAK = false;
      RUNNING = true;
      error_mess = NULL;
      break;

    case '?':
      ui_welcome();
      break;

    case 'L':
      tm.Minute = 0;
      tm.Second = 0;
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      n = atoi(tok);
   
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      incHr = atoi(tok);
      
      
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Year = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Month = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Day = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Hour = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Minute = atoi(tok);
      tok = strtok(NULL, " ");
      if (tok == NULL) goto OK2;
      tm.Second = atoi(tok);

OK2:
      moon_printTable(tm,n,incHr);
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
  if (BREAK) return true;
  char c = myraw(false);
  if (c == 0) return false;
  ui_fatal(F(" User BREAK. "));
  return true;
}



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
    if (inChar == 13) {
      ui_command(input_string);
      str_cnt = 0;
    } else {
      if (str_cnt < MAX_CHAR - 1) str_cnt++;
    }
  }
}

// Poll for input via USB or BT.
// Build up a string.
// When a CR is recieved process the command


void ui_poll() {
  
  if (BREAK) {
    ui_printError();
    BREAK = false;
  }

  ui_poll2();
}

