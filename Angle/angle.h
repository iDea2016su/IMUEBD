#ifndef ANGLE_H
#define ANGLE_H
#include "sys.h"

u8 getRotationMatrix(double gravity[], double geomagnetic[]);
void getOrientation(double gravity[], double geomagnetic[]);
double getAngle360(double anglePi);
void printAttitude(double a[], double m[]);
double getInclination(void); 
float Data_conversion(float *AccBuffer,float *MagBuffer);

void setAngle(float a);
float getAngle(void);
#endif