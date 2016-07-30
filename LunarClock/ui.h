#ifndef _UI_H
#define _UI_H

#include "mytime.h"
#include <Arduino.h>


extern bool RUNNING;
extern int  ui_tilt;
extern int  ui_phase;

// Display error string
bool ui_printError();

// Visit the Serial inputs
// If CR then process command.
void ui_poll();

void ui_welcome();

// set the error string
//         RUNNING false 
//         BREAK   true
void ui_fatal(const __FlashStringHelper *mess);



// There is an uncleared error message
bool ui_hasError();

// has user entered break characcter ?
// if so BREAK is also set.   YOu may want to set RUNNING false after detecting this!!!
bool ui_poll_break();

#endif
