#ifndef ANGLE_H
#define ANGLE_H
#include "sys.h"

u8 getRotationMatrix(float gravity[], float geomagnetic[]);
void getOrientation(float gravity[], float geomagnetic[], float angle[]);
float getAngle360(float anglePi);
void printAttitude(float a[], float m[]);
float getInclination(void); 

#endif