#ifndef _ANG_H
#define _ANG_H

extern const float deg2rad;

// return angle in range -180 180
float ang_wrap(float val);

// return phase diff (p1-p2) wraped to [-180, 180] range
float ang_diff(float p1, float p2);

// rotate about Z,x,y
// can use for   Y,z,x  Z,y,z


void rotate(float angZ, float &vxnew, float &vynew, float &vznew);

class Vector3D {

  public:

    float _x, _y, _z;

    Vector3D(float x = 0, float y = 0, float z = 0) {
      _x = x;
      _y = y;
      _z = z;
    }

    void rotX(float ang);
    void rotY(float ang);
    void rotZ(float ang);

};



#endif
