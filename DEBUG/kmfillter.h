#ifndef _KM_H
#define _KM_H

#include "sys.h"

float filterA(float z);
float filterB(float z);
float filterC(float z);

float filterStepX(float z);
float filterStepY(float z);
float filterStepZ(float z);

float filterAngleX(float z);
float filterAngleY(float z);
float filterAngleZ(float z);

#endif

