#ifndef _TILT_H
#define _TILT_H

// Interface to the tilt servo system.

#define SERVO_MAX_ATTACH_TIME  400
#define SERVO_RETRY_TIME    50
#define SERVO_MAX_TRY 10
#define LOW_VOLT 4700


// absolute SERVO LIMITS in microseconds for servo pulse
// IF SERVO is properly zeroed should avoid hitting grahams nasty end stops.
#define SERVO_RANGE 550
#define SERVO_MID  1500
#define SERVO_MIN (SERVO_MID-SERVO_RANGE)
#define SERVO_MAX (SERVO_MID+SERVO_RANGE)

#define TILT_ANG_UP   50
#define TILT_ANG_DOWN (-TILT_ANG_UP)

extern int servo_retry_count;
extern int servo_pos;
extern bool tilt_low_volt;

// Initialise tilt system.
void tilt_setup();

// Set tilt.  Keep servo attached or onTime millisecs  (0-infinfite)
void tilt_set(float tilt, int onTime);

// Is servo active?
bool tilt_running();

// set the limits of servo in uS
void tilt_setLimits(int down, int up);



#endif
