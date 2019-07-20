#include "stepCount.h"
#include "FreeRTOS.h"			 
#include "task.h"


static int mCount = 0;  //当前步数
static int count = 0;   //缓存步数,步数3秒内小于10步则不计步
static long timeOfLastPeak = 0; //计时，时间窗口开始时间
static long timeOfThisPeak = 0;

void countStep()
{
	timeOfLastPeak = timeOfThisPeak;
	timeOfThisPeak = xTaskGetTickCount();
	if(timeOfThisPeak - timeOfLastPeak <= 10000)
	{
		if(count < 2)
		{
			count++;
		}
		else if(count == 2)
		{
			count++;
			mCount += count;
		}
		else
		{
			mCount++;
			printf("step: %d\r\n",mCount);
		}
	}
	else
	{
		count = 1;
	}
}

void setStep(int initNowBusu)
{
		mCount = initNowBusu;//接收上层调用传递过来的当前步数
		count = 0;
		timeOfLastPeak = 0;
		timeOfThisPeak = 0;
}
