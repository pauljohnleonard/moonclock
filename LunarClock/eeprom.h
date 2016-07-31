#ifndef _EEPROM_H
#define _EEPROM_H


// set microseconds for MAX and MIN ANG
extern int tilt_servoDownLimit;
extern int tilt_servoUpLimit;

// phase offset for moon in degs
extern  int phase_Offset;

// SENSOR CALIBRATION
extern int sensor1_max;
extern int sensor1_min;
extern int sensor2_max;
extern int sensor2_min;


void eeprom_write();
void eeprom_read();


#endif
