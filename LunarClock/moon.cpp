
#include <Arduino.h>
#include "moon.h"
#include "ang.h"
#include "data.h"
#include "mytime.h"
#include "global.h"
#include "myio.h"
#include "ui.h"
#include <math.h>


float moon_phase;
float moon_tilt;
float moon_alt;
float season_ang;

// return phase of moon given calendar time
static float get_moon_phaseRad(MyTime &tm) {


  long jhPrev = data_jh_prev();
  long jhNext = data_jh_next();

  long dJH = jhNext - jhPrev;

  long bJH = tm.getJH() - jhPrev;

  float ang = M_PI * (bJH + tm.getMinute() / 60.) / dJH;

  if (data_isWaxing()) return ang;

  else return ang - M_PI;

}


void moon_setup() {

}


void _testCase(int i, float &angOfTilt, float &tilt, float &latitude, float &angOfMoon)
{

  switch (i) {

    case 0:
      angOfTilt = 0;
      tilt = 45 * deg2rad;
      latitude = 90 * deg2rad;
      angOfMoon = M_PI / 2;
      break;
    // tilt= -45  alt=0

    case 1:
      angOfTilt = 0;
      tilt = 45 * deg2rad;
      latitude = 90 * deg2rad;
      angOfMoon = 3 * M_PI / 2;
      // tilt=45  alt=0
      break;

    case 2:
      angOfTilt = 0;
      tilt = 0 * deg2rad;
      latitude = 45 * deg2rad;
      angOfMoon = M_PI / 2;
      break;
    // t=0     tilt=-45    alt=0
    //   6            0       45
    //  12          45         0
    //  18           0       -45

    case 3:
      angOfTilt = M_PI / 2;
      tilt = 45 * deg2rad;
      latitude = 90 * deg2rad;
      angOfMoon = M_PI / 2;
      break;
    // tilt=0  alt=45

    case 4:
      angOfTilt = M_PI / 2;
      tilt = 45 * deg2rad;
      latitude = 45 * deg2rad;
      angOfMoon = M_PI / 2;
      break;
    // t= 0   -54.7  30
    // t= 6    ?     90
    // t=12   54.7   30
    // t= 18   0     0

    case 5:
      angOfTilt = 0;          // wint sol
      angOfMoon = M_PI / 2;  // half moon
      break;

    // t=0     tilt=-61.24  ((90-latitude)+tilt)  alt=0
    // t=12         15      ((90-latitude)-tilt)  alt=0

    case 6:
      angOfTilt = M_PI;          // summer sol
      angOfMoon = M_PI / 2;  // half moon
      break;

    // t=0     tilt=-15     alt=0
    // t=12         61.2    alt=0

    case 7:
      angOfTilt = M_PI / 2;  // equinox (which one !!!)
      angOfMoon = M_PI / 2; // half moon
      break;

    // 6       0.000    62.209
    // 18      0        -15

    case 8:
      angOfTilt = 0 ;
      tilt = 45 * deg2rad;
      latitude = 90 * deg2rad;
      angOfMoon = M_PI;
      break;
    // tilt=0 alt=-45

    default:
      break;
  }
}




//  angOfMoon relative to new moon in deg  e.g. 0 is new 180 is full  90 is first quarter
//
static void  set_moon_tilt(MyTime t, float angOfMoon)

{

  // TODO improve me
  static MyTime winterSoltice(2000, 12, 21, 0, 0, 0);
  static const float timeTiltRef = winterSoltice.dayOfYear();    //  when tilt away from sun (north hemi) is MAX (winter solstice)


  static float latitude = 51.2308 * deg2rad;  //  degrees from equator  (deg).
  static float tilt = 23.44 * deg2rad;       //  earths axis tilt from (deg).
  static const float julianYear = 365.24;                      //   ?? is this right ?


  // ang of Full moon is reference (zero)
  // So angOfMoon is between (vector radial from sun) TO  (vector position of moon relative to earth).
  //   0 is full moon
  //   !PI is new moon.

  angOfMoon += M_PI;

  float timeOfDay    =  t.getFractionOfDay();                  // 0 - 1 from midnight
  float timeOfSeason =  t.dayOfYear() - timeTiltRef;           // days after winter solstice
  if (timeOfSeason < 0) timeOfSeason += 365.25;

  float angOfDay  = timeOfDay * 2 * M_PI;                    // rotation of earth  from midnight in rads

  // component of tilt on obrital plane  relative to radial from the sun.
  float angOfTilt = -(timeOfSeason / julianYear) * 2 * M_PI; // phase of the season (this is going backwards).

  season_ang = ang_wrap(-angOfTilt / deg2rad); // Just for user feedback  see ui.cpp

  // Don't belive the results then try some easy vcalues for a sanity check!!!!
  // _testCase(8,angOfTilt,tilt,latitude,angOfMoon);


  // frame of reference:
  // origin is earths core.
  // y direct earth orbit path?
  // x is radial from sun.
  // z is normal to orbit plane.   (everything spins right hand around Z)


  // initialize  POV vertical vector at observation point.
  // longitude is =0.   (Near enough for frome ?) Infact you should correct time if you are fussed.
  // time is midnight t=0.
  // Earth is at its origin (no tilt yet).

  // Here we vector calculusy go . . . .

  Vector3D POV(cos(latitude), 0, sin(latitude));

  // Rotate POV according to time of day.
  POV.rotZ(angOfDay);

  // Now do the tilt.

  // Transform to new coordinate system by rotating around the Z axis
  // such that the new Y  (Yd)  is right angles to tilt axis and Z axis    --- > Vd1
  // that is new x is pointing tilt direction
  // angoFtilt is angle from x (raially out from sun) to projection of tilt vector onto earthorbit plane
  // -ve sign because we are rotating coordinate system

  POV.rotZ(-angOfTilt);

  // Rotate by tilt around new Y axis

  POV.rotY(tilt);

  // Transform back to original coordintate system.

  POV.rotZ(angOfTilt);

  // NOW POV IS IN IT'S REAL POSITION .

  // Transform  coordinate system about Z AXIS  such that new X  is towards the moon.
  // e.g. rotate cordinate by angOfMoon  (or rotate system by -angMoon)
  // POV_x gives us angle between our vertical and direction to the moon  1 is directly overhead  0 is on horizon
  // The angle of the phase axis is given by Vmy and Vmz

  POV.rotZ(-angOfMoon);

  // tilt of moon in degs clockwise form vertical  from POV.
  // alt degrees above horizon.

  moon_tilt = ang_wrap(  atan2(POV._y, POV._z) / deg2rad);
  moon_alt =  asin(POV._x) / deg2rad;

}

// sets up global variables moon_phase and moon_tilt

void moon_updateState(MyTime &t) {
  data_setIndexAt(t.getJH());
  float ang = get_moon_phaseRad(t);
  set_moon_tilt(t, ang);
  moon_phase = 180.*ang / M_PI;
}


void moon_printTable(MyTime t, int n, int incHr) {

  myprintf(F(" Date        time    season       phase      tilt      alt \n"));

  for (int i = 0; i < n ; i++) {
    if (ui_poll_break()) return;
    moon_updateState(t);
    t.print(NULL);
    myprintf(F(" %s "), myf2str(season_ang));
    myprintf(F(" %s "), myf2str(moon_phase));
    myprintf(F(" %s "), myf2str(moon_tilt));
    myprintf(F(" %s \n"), myf2str(moon_alt));

    t = MyTime(t.getJH() + incHr, t.getMinute(), t.getSecond());
  }
}

void moon_halfTable(int n) {

  for (int i = 0; i < n ; i++) {
    if (ui_poll_break()) return;
    MyTime tp = MyTime(data_jh_prev(), 0, 0);

    if (data_isWaxing()) {
      tp.printJD(F("  New: "));
      myprintln();
    } else {
      tp.printJD(F(" Full: "));
      myprintln();
    }
    data_inc();
  }
}



