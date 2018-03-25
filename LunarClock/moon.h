#ifndef _MOON_H
#define _MOON_H

#include "mytime.h"
#include <Time.h>

// State of moon in degrees  -180 to 180   (I hope)
extern float moon_phase;
extern float moon_tilt;
extern float moon_alt;
extern float season_ang;

// Update the state of the moon
void moon_updateState(MyTime &t);
void moon_printTable(MyTime t, int n, int incHr);
void moon_halfTable(int n);

#endif
