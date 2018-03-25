#include <stdarg.h>
#include <Arduino.h>
#include "stdio.h"

Stream *serial = NULL;

#define PRINTF_BUF 128

static char buf[PRINTF_BUF];


char * myf2str(float val) {

  static char dbuff[20];
  dtostrf(val, 8, 3, dbuff);
  return dbuff;
}

void writeBuf(Stream *x, char * buff) {

  for (char *p = &buff[0]; *p ; p++)
  {
    if (*p == '\n') {
      x->write('\r');
    }
    x->write(*p);
  }
}



void myprintf(const char *fmt, ... ) {
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, PRINTF_BUF, fmt, args);


  writeBuf(serial, buf);

  va_end (args);

}


#ifdef F // check to see if F() macro is available
void myprintf(const __FlashStringHelper *format, ...)
{
  va_list ap;
  va_start(ap, format);

#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
#endif


  writeBuf(serial, buf);
  va_end(ap);

}
#endif


void myprintln() {

  myprintf(F("\n"));
  // writeBuf(serial, '\n');

}

void myflush() {
  delay(10);
  while (serial->available()) {
    char  c = serial->read();
    delay(10);
  }
}

char  myraw(bool wait) {

  while (true) {
    if (serial->available()) {
      char  c = serial->read();
      serial->write(c);
      return c;
    }
    if (!wait) return 0;
  }
}

