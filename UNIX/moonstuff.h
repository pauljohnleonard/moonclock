#ifndef CALENDAR_H
#define CALENDAR_H


typedef double MYDOUBLE;

#include<ctime>
#include<cmath>

using namespace std;

struct calendar 
{
  int yy;
  int mm;
  int dd;
  int hr;
  int min;
  int sec;
  long jh;
};

calendar JDtoCalendar(MYDOUBLE jd); //converts julian date to calendar date 
MYDOUBLE calendarToJD(calendar cal);//converts calendar to julian date
void calcNewMoonPhase(const calendar cur, calendar &prev,calendar &next);
calendar  calcNewMoonPhaseK(MYDOUBLE k);
calendar  calcFullMoonPhaseK(MYDOUBLE k);
void calcFullMoonPhase(calendar cur,calendar &prev,calendar &next);
void getCurMoonPhase(calendar &prevN,calendar &nextN ,calendar &prevF, calendar &nextF);
void getThisMoonPhase(int dd,int mm,int yy,calendar &prevN,calendar &nextN ,calendar &prevF, calendar &nextF);
MYDOUBLE cal2Kish(calendar c);


#endif
