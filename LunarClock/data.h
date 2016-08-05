#ifndef _DATA_H
#define _DATA_H

// Interface to the program memory data that stores the Moon phasetable.

void data_setup();

void data_inc();
void data_dec();

// Set the current date using julian hours   24* (julian day)   Duhh!!!
void data_setIndexAt(long jh);

long data_jh_prev();     // Julian hours of previous full/new moon
long data_jh_next();     // Julian hours of next full/new moon

// True if prev is new moon;
bool data_isWaxing();

int data_index();

void data_welcome();

#endif


