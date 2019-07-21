#ifndef _KM_H
#define _KM_H

#include "sys.h"

float filter(float z);

float filterA(float z);
float filterB(float z);
float filterC(float z);

float filterStepX(float z);
float filterStepY(float z);
float filterStepZ(float z);

float filterAngleX(float z);
float filterAngleY(float z);
float filterAngleZ(float z);

float filterMX(float z);
float filterMY(float z);
float filterMZ(float z);

#endif

