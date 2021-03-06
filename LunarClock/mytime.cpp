
#include <Arduino.h>
#include <DS1307RTC.h>

#include "mytime.h"
#include "ui.h"
#include "myio.h"


// 0 use RTC.
// Other wise clock is N mins per real sec


int mytime_speed = 0;


static MyTime *myTimeRef = NULL;
static long millis_ref;

void mytime_setSpeed(int i) {

  if (mytime_speed == 0) {  // set my clock to wall time
    if (myTimeRef == NULL) myTimeRef = new MyTime();
    myTimeRef->readRTC();
    myTimeRef->setTimeFromJH(myTimeRef->getJH(), myTimeRef->getMinute(), 0); //
  }

  mytime_speed = i;
  millis_ref = millis();

}


int MyTime::dayOfYear() {

  int days = Day;

  for (int m = 1; m < Month; m++) {
    days += mytime_daysInMonth(m, Year);
  }

  return days;

}

MyTime::MyTime() {

  if (mytime_speed == 0) {
    readRTC();
  } else {
    int deltaSec = (millis() - millis_ref) / 1000;
    int clockMins = mytime_speed * deltaSec;
    long JH = myTimeRef->JH + clockMins / 60;

    setTimeFromJH(JH, clockMins % 60, 0);
  }
}

int mytime_daysInMonth(int month, int year)
{
  int numberOfDays;
  if (month == 4 || month == 6 || month == 9 || month == 11)
    numberOfDays = 30;
  else if (month == 2)
  { bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeapYear)
      numberOfDays = 29;
    else
      numberOfDays = 28;
  }
  else
    numberOfDays = 31;

  return numberOfDays;
}

// See: http://jtds.sourceforge.net/doc/net/sourceforge/jtds/jdbc/DateTime.html

void MyTime::setJHfromCal()
{
  int YEAR = this->Year;
  int MONTH = this->Month;
  int DAY = this->Day;
  int HOUR = this->Hour;

  long I, J, K, JD;

  I = YEAR;
  J = MONTH;
  K = DAY;

  JD = K - 32075 + 1461 * (I + 4800 + (J - 14) / 12) / 4 + 367 * (J - 2 - (J - 14) / 12 * 12) / 12 - 3 * ((I + 4900 + (J - 14) / 12) / \
       100) / 4;


  this->JH = JD * 24 + HOUR - 12;

}



MyTime::MyTime(long JH1, int mins, int secs) {


  setTimeFromJH(JH1, mins, secs);

}

void MyTime::setTimeFromJH(long JH1, int mins, int secs) {

  long I, J, K, L, N;

  this->JH = JH1;

  JH1 = JH1 + 12;

  long JD = JH1 / 24;

  Hour = JH1 % 24;

  Minute = mins;
  Second = secs;

  L = JD + 68569;
  N = 4 * L / 146097;
  L = L - (146097 * N + 3) / 4;
  I = 4000 * (L + 1) / 1461001;
  L = L - 1461 * I / 4 + 31;
  J = 80 * L / 2447;
  K = L - 2447 * J / 80;
  L = J / 11;
  J = J + 2 - 12 * L;
  I = 100 * (N - 49) + I + L;

  Year = I;
  Month = J;
  Day = K;


  MyTime t2 = MyTime(I, J, K, Hour, Minute, Second);

  if (t2.JH != JH) {
    myprintf(F(" Bug in MyTime constructor from JH\n"));
    printJD(F("A "));
    t2.printJD(F("\nB "));
    myprintln();
  }

}


MyTime::MyTime(int y, int m, int d, int h, int minute, int second) {

  Minute = minute;
  Second = second;
  Year = y;
  Month = m;
  Day = d;
  Hour = h;
  setJHfromCal();
}

bool MyTime::parseStr(char *str) {

 // myprintf(F(" >%s< \n"),str);

  int yy,mm,dd,hr,mn,sc=0;
  
  char *tok = strtok(str, " ");
  if (tok == NULL) return false;
  yy = atoi(tok);
  tok = strtok(NULL, " ");
  if (tok == NULL) return false;
  mm = atoi(tok);
  tok = strtok(NULL, " ");
  if (tok == NULL) return false;
  dd = atoi(tok);
  tok = strtok(NULL, " ");
  if (tok == NULL) return false;
  hr = atoi(tok);
  tok = strtok(NULL, " ");
  if   (tok == NULL) return false;
  mn = atoi(tok);
  tok = strtok(NULL, " ");
  if (tok != NULL) sc= atoi(tok);

  Year=yy;
  Month=mm;
  Day=dd;
  Hour=hr;
  Minute=mn;
  Second=sc;
  setJHfromCal();
  return true;
  
}

//
//MyTime::MyTime(char *str) {
//  MyTime();
//  Hour = Minute = Second = 0;
//  parseStr(str);
//  setJHfromCal();
//}



bool MyTime::readRTC() {

  tmElements_t tm;

  if (RTC.read(tm)) {
    Year = tmYearToCalendar(tm.Year);
    Month = tm.Month;
    Day = tm.Day;
    Hour = tm.Hour;
    Second = tm.Second;
    Minute = tm.Minute;
    setJHfromCal();
    return true;
  }


  if (RTC.chipPresent()) {
    ui_fatal(F("The RTC is stopped. Unable to read time \n"));
  } else {
    ui_fatal(F("RTC read error!  Is it connected? Please check the circuitry. \n"));
  }
  return false;
}

bool MyTime::writeRTC() {

  tmElements_t t;

  t.Year = CalendarYrToTm(Year);
  t.Month = Month;
  t.Day = Day;
  t.Hour = Hour;
  t.Minute = Minute;
  t.Second = Second;

  if (RTC.write(t)) return true;


  if (RTC.chipPresent()) {
    ui_fatal(F("The RTC is stopped. Unable to write time."));
  } else {
    ui_fatal(F("RTC write error! Is it connected?  Please check the circuitry."));
  }

  return false;
}



void MyTime::print(const __FlashStringHelper  *tag) {

  if (tag != NULL)  myprintf(tag);
  myprintf(F("%02d/%02d/%04d %2d:%02d:%02d"), Day, Month, Year, Hour, Minute, Second);

}


void MyTime::printJD(const __FlashStringHelper  *tag) {

  print(tag);

  long JD = JH / 24;
  long hrs = JH % 24;

  int  milliDay = round(1000.0 * (hrs + Minute / 60.0) / 24.0);
  myprintf(F(" JD=%ld.%03d "), JD, milliDay);

}
