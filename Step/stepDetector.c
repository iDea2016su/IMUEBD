#include "stepDetector.h"
#include "math.h"
#include "FreeRTOS.h"			 
#include "task.h"
#include "stepCount.h"

float oriValues[3] = {0};
float gravityNew = 0;
float gravityOld = 0;
u8 lastStatus = 0;
u8 isDirectionUp = 0;
int continueUpCount = 0;
int continueUpFormerCount = 0;
float peakOfWave = 0;
float valleyOfWave = 0;
long timeOfThisPeak = 0;
long timeOfLastPeak = 0;
long timeOfNow = 0;
int TimeInterval = 350;   //波峰波谷时间差
float ThreadValue = (float) 2.0;
float InitialValue = (float) 1.3;

int tempCount = 0;
#define ValueNum  8
float tempValue[ValueNum] = {0};

void sensorChanged(float ax,float ay, float az)
{
	oriValues[0] = ax/30;
	oriValues[1] = ay/30;
	oriValues[2] = az/30;
	gravityNew = sqrt(oriValues[0]*oriValues[0]+oriValues[1]*oriValues[1]+oriValues[2]*oriValues[2]);
	detectorNewStep(gravityNew);
}

void detectorNewStep(float values) 
{
        if (gravityOld == 0) 
				{
            gravityOld = values;
        } 
				else 
				{
            if (detectorPeak(values, gravityOld)) 
							{
                timeOfLastPeak = timeOfThisPeak;
                timeOfNow = xTaskGetTickCount();
                if (timeOfNow - timeOfLastPeak >= TimeInterval
                        && (peakOfWave - valleyOfWave >= ThreadValue)) {
                    timeOfThisPeak = timeOfNow;
                    countStep();
                }
                if (timeOfNow - timeOfLastPeak >= TimeInterval
                        && (peakOfWave - valleyOfWave >= InitialValue)) {
                    timeOfThisPeak = timeOfNow;
                    ThreadValue = peakValleyThread(peakOfWave - valleyOfWave);
                }
            }
        }
        gravityOld = values;
    }

u8 detectorPeak(float newValue, float oldValue) 
{
    lastStatus = isDirectionUp;
    if (newValue >= oldValue) 
		{
				isDirectionUp = true;
				continueUpCount++;
		} 
		else 
		{
				continueUpFormerCount = continueUpCount;
				continueUpCount = 0;
				isDirectionUp = false;
		}

		if (!isDirectionUp && lastStatus && (continueUpFormerCount >= 2 || oldValue >= 20)) 
		{
				peakOfWave = oldValue;
				return true;
		} else if (!lastStatus && isDirectionUp) {
				valleyOfWave = oldValue;
				return false;
		} else {
				return false;
		}
}

float peakValleyThread(float value) 
{
		float tempThread = ThreadValue;
		if (tempCount < ValueNum) {
				tempValue[tempCount] = value;
				tempCount++;
		} else {
				tempThread = averageValue(tempValue, ValueNum);
				for (int i = 1; i < ValueNum; i++) {
						tempValue[i - 1] = tempValue[i];
				}
				tempValue[ValueNum - 1] = value;
		}
		return tempThread;

}

     /* 梯度化阈值
     * 1.计算数组的均值
     * 2.通过均值将阈值梯度化在一个范围里
     * */
float averageValue(float value[], int n) 
{
		float ave = 0;
		for (int i = 0; i < n; i++) {
				ave += value[i];
		}
		ave = ave / ValueNum;
		if (ave >= 8)
				ave = (float) 4.3;
		else if (ave >= 7 && ave < 8)
				ave = (float) 3.3;
		else if (ave >= 4 && ave < 7)
				ave = (float) 2.3;
		else if (ave >= 3 && ave < 4)
				ave = (float) 2.0;
		else {
				ave = (float) 1.3;
		}
		return ave;
}