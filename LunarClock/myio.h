#ifndef _MYIO_H
#define _MYIO_H

#include <Arduino.h>

extern Stream * serial;

void myprintf(const char *fmt, ... );
void myprintf(const __FlashStringHelper *format, ...);
char myraw(bool wait);
void myprintln();
void myflush();
char * myf2str(float val);

#endif
