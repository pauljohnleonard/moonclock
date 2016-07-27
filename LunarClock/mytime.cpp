
#include <Arduino.h>
#include <DS1307RTC.h>

#include "mytime.h"
#include "ui.h"

int mytime_dayOfYear(MyTime t) {
  
  int days=t.Day;
  
  for (int m=1;m<t.Month;m++) {
      days+=mytime_daysInMonth(m,t.Year);
  }

  return days;
  
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

void mytime_setJHfromCal(MyTime &t)
{
  int YEAR=t.Year;
  int MONTH=t.Month;
  int DAY=t.Day;
  int HOUR=t.Hour;
  
  long I, J, K, JD;

  I = YEAR;
  J = MONTH;
  K = DAY;

  JD = K - 32075 + 1461 * (I + 4800 + (J - 14) / 12) / 4 + 367 * (J - 2 - (J - 14) / 12 * 12) / 12 - 3 * ((I + 4900 + (J - 14) / 12) / \
       100) / 4;

  t.JH = JD * 24 + HOUR - 12;
}




void  mytime_setCalfromJH(MyTime &t)
{
  long I, J, K, L, N, JD;
  long JH=t.JH;
  JH=JH+12;
  JD = JH / 24;
  t.Hour = JH % 24;
  t.Minute=0;
  t.Second=0;
  
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

  t.Year = I;
  t.Month = J;
  t.Day = K;
  
}


MyTime mytime_makeTime(int y,int m,int d,int h) {
  MyTime t;
  t.Minute=t.Second=0;
  t.Year=y;
  t.Month=m;
  t.Day=d;
  t.Hour=h;
  return t;
}
//
//void mytime_test() {
//
//  int YEAR,MONTH,DAY,HOUR;
//  for (int y = 2016; y < 2090; y++) {
//    Serial.println(".");
//    for (int m = 1; m < 13; m++) {
//      for (int d = 1; d <= mytime_daysInMonth(m, y); d++) {
//        for (int h = 0; h < 24; h++) {
//            
//            long JH=mytime_cal2JH(mytime_makeTime(y,m,d,h));
//            MyTime t;
//            mytime_JH2Cal(JH, t);
//            if (t.Year !=y || t.Day != d || t.Month != m || t.Day !=d || t.Hour != h) {
//             
//                Serial.println("  EEEEK ");            
//            } else {
//             // 
//            }
//        }
//      }
//
//    }
//  }
//

// }

bool mytime_read(MyTime &t) {
  tmElements_t tm;
  if (RTC.read(tm)) {

    t.Year = tmYearToCalendar(tm.Year);
    t.Month = tm.Month;
    t.Day = tm.Day;
    t.Hour = tm.Hour;
    t.Second = tm.Second;
    t.Minute = tm.Minute;
    return true;
  }


  if (RTC.chipPresent()) {
    ui_fatal(F("The DS1307 is stopped. \n"));
  } else {
    ui_fatal(F("DS1307 read error!  Please check the circuitry. \n"));
  }

  return  false;

}

bool mytime_write(MyTime tm) {

  tmElements_t t;

  t.Year = CalendarYrToTm(tm.Year);
  t.Month = tm.Month;
  t.Day = tm.Day;
  t.Hour = tm.Hour;
  t.Minute = tm.Minute;
  t.Second = tm.Second;


  if (RTC.write(t)) return true;
  
  
  if (RTC.chipPresent()) {   
    ui_fatal(F("The DS1307 is stopped. \n"));
  } else {
    ui_fatal(F("DS1307 read error!  Please check the circuitry. \n"));
  }
 
  return false;
}



