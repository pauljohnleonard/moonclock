#ifndef _MYTIME_H
#define _MYTIME_H


// valid for years 2000 - 2099

typedef struct {
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
  long JH;
} MyTime;



// days into the year;
int mytime_dayOfYear(const MyTime t);
int mytime_daysInMonth(int m,int y);


// Convert date into Julian Hours  
void  mytime_setJHfromCal(MyTime &t);
void  mytime_setCalfromJH(MyTime &t);

// READ TIME FOR HARDWARE CLOCK:  
bool mytime_read(MyTime &t);         

// WRITE TIME TO HARDWARE CLOCK      
bool mytime_write(MyTime t);

void mytime_test();

MyTime mytime_makeTime(int y,int m,int d,int h);

#endif




