#ifndef _EEPROM_H
#define _EEPROM_H


// set microseconds for 70deg and -70deg points
extern int tilt_PosDown;
extern int tilt_PosUp;

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
