#ifndef _TILT_H
#define _TILT_H

#include "Arduino.h"


// SERVO LIMITS in microseconds for servo pulse
#define SERVO_RANGE 600
#define SERVO_MID 1500
#define SERVO_MIN (SERVO_MID-SERVO_RANGE)
#define SERVO_MAX (SERVO_MID+SERVO_RANGE)

#define TILT_ANG_UP   70
#define TILT_ANG_DOWN (-TILT_ANG_UP)


extern int servo_pos;

void tilt_setup();

// Set tilt.  Keep servo attached or onTime millisecs  (0-infinfite)
void tilt_set(float tilt,int onTime);


// set the limits of servo in uS
void tilt_setLimits(int down,int up);



#endif