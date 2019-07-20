#ifndef DETECTOR_H
#define DETECTOR_H

#include "sys.h"


void detectorNewStep(float values) ;
u8 detectorPeak(float newValue, float oldValue);
float averageValue(float value[], int n);
float peakValleyThread(float value);
void sensorChanged(float ax,float ay, float az);
#endif