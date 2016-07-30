#ifndef _MYTIME_H
#define _MYTIME_H

#include <Arduino.h>
#include <DS1307RTC.h>

// valid for years 2000 - 2099

class MyTime
{
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
  long JH;

public:
  MyTime(int y,int m,int d,int h,int minute,int second);
  MyTime(long JH,int mins,int secs);
  MyTime();
  MyTime(char *str);

  // days into the year;
  int dayOfYear();


  // READ TIME FOR HARDWARE CLOCK:  
  //bool r();         

  // WRITE TIME TO HARDWARE CLOCK      
  bool write();

  long getJH() { return JH;}   
  int getYear() {return Year;}
  int getMonth() {return Month;}
  int getDay() {return Day;}
  int getMinute() { return Minute;} 
  int getSecond() { return Second;}
  int getHour() {return Hour;}
  float getFractionOfDay() { return (Hour + (Minute+ Second/ 60.0) / 60.0) / 24.0 ;} 
  void print(const __FlashStringHelper *tag);
  void printJD(const __FlashStringHelper *tag);
private:
  // Convert date into Julian Hours  
  void  setJHfromCal();
  void  setCalfromJH();
  void parseStr(char *str);



};



int mytime_daysInMonth(int m,int y);



#endif




