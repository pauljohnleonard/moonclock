

#include "ang.h"

#include "math.h"


const float deg2rad = M_PI / 180.0;

// return phase diff wraped to [-180, 180] range
float ang_diff(float p1, float p2) {

  float dp = p1 - p2;
  return ang_wrap(dp);

}

float ang_wrap(float val) {

  while (val > 180) val -= 360;
  while (val < -180) val += 360;
  return val;
}


void rotate(float ang, float &vx, float &vy) {

  float vx_tmp = vx;
  vx = vx * cos(ang) - vy * sin(ang);
  vy = vx_tmp * sin(ang) + vy * cos(ang);

}


void Vector3D::rotX(float ang) {
  rotate(ang, _y, _z);
}

void Vector3D::rotY(float ang) {
  rotate(ang, _z, _x);
}

void Vector3D::rotZ(float ang) {
  rotate(ang, _x, _y);
}

