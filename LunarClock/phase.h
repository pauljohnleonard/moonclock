#ifndef _PHASE_H
#define _PHASE_H

#include "Arduino.h"


extern int phase_sense1;
extern int phase_sense2;
extern bool PHASE_BROKEN;

void phase_test();

// spin the moon and record max / min sensor values
void phase_calibrate();

// return phase in degs   [-180,180] range
float phase_read();

// motor back
void phase_back();


// motor forward
void phase_forward();


// halt the motor
void phase_halt();

// initialize
void phase_setup();



//  add this angle to all phase settings
void phase_setOffset(int value);


// Set the phase of the moon
void phase_set(int phase);

#endif
