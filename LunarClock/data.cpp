#include <Arduino.h>
#include "progmem.h"
#include "ui.h"
#include "mytime.h"
#include "global.h"
#include "myio.h"

static long jh_prev;
static int  jh_index;
static long jh_next;
static int  n_djh;

// Get difference in julian hours for currrent half period.
// Add OFFSET to value stored in PROGMEM for current index.
static int get_djh() {
  byte b = pgm_read_byte_near(_djh + jh_index);
  return _dfh_off + b;
}


void data_welcome() {
  myprintf(F("** MOON PHASE TABLE\n"));
  myprintf(F(" JH table size  : %d\n"), n_djh);
  MyTime t = MyTime(yy_ref, mm_ref, dd_ref, hr_ref, 0, 0);
  t.print(F(" First New Moon :"));
  myprintln();
}

void debug(const char *X) {
  // myprintf(F("%s %d %ld %ld\n"),X,jh_index,jh_prev,jh_next);
}


void data_setup() {
  n_djh = sizeof(_djh);
  MyTime t = MyTime(yy_ref, mm_ref, dd_ref, hr_ref, 0, 0);
  jh_index = 0;
  jh_prev = t.getJH();
  jh_next = jh_prev + get_djh();
}


// Advance table pointer

void data_inc() {

  if ( jh_index  >=  (n_djh - 1) ) {
    ui_fatal(F(" CLOCK is after end of table "));
    return;
  }

  jh_index++;
  jh_prev = jh_next;
  jh_next = jh_prev + get_djh();

}


// Decrement table pointer

void data_dec() {

  if (jh_index  <= 0) {
    ui_fatal(F(" CLOCK is before start of table "));
    return;
  }

  jh_index--;
  jh_next = jh_prev;
  jh_prev = jh_next - get_djh();
}



// adjust jh_index to point to entry less than jh  whilst jh_index+1   is greater
// even index is a new noon.

void  data_setIndexAt(long jh) {

  if (ui_hasError() != 0 ) return;

  while ( ( jh_prev   >  jh ) && (! ui_hasError() ) ) {
    data_dec();
  }

  while ( ( jh_next   <  jh ) && ( !ui_hasError() ) ) {
    data_inc();
  }

}


// Date of half phase prior to current time
long data_jh_prev() {
  return jh_prev;
}

// Date of half phase after to current time
long data_jh_next() {
  return jh_next;
}


// True if prev is new moon;
bool data_isWaxing() {
  return (jh_index % 2) == 0;
}


int data_index() {
  return jh_index;
}

