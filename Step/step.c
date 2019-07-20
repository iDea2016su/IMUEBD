#include "step.h"
#include "FreeRTOS.h"			 
#include "task.h"
#include "Queue.h"
#include "scope.h"

#define QueueLen 500
#define BufSize  40
QueueHandle_t dataQueue; 

void stepTask(void *pArg)
{
	dataQueue = xQueueCreate(QueueLen,sizeof(float));
	vTaskDelay(100);
	while(1)
	{
		float p = 0;
		xQueueReceive(dataQueue,&p,0);
		float var = getVar(10,p);
		float order = getOrder(40,p);
		float res = var*order;
		send(var,order,res,p);
		vTaskDelay(5);
	}
}


void sendData(float p)
{
	BaseType_t err;
	err = xQueueSend(dataQueue,&p,0);
}

float getVar(int num,float value)
{
	static float arr[BufSize] = {0};
	static int pos = 0;
	arr[pos%num] = value;
	float sum = 0;
	for(int i=0;i<num;i++)
	{
		sum += arr[i];
	}
	float avr = sum/num;
	float var = 0.0;
	for(int i=0;i<num;i++)
	{
		var += (arr[i]-avr)*(arr[i]-avr);
	}
	pos++;
	
	return 0.01*var/num;
}

float getOrder(int num,float value)
{
  static float arr[BufSize] = {0};
	static int pos = 0;
	arr[pos%num] = value;
  float max = value;
	float min = value;
	for(int i=0;i<num;i++)
	{
		if(max < arr[i])
		{
			max = arr[i];
		}
    if(min > arr[i])
		{
			min = arr[i];
		}
	}
	float order = 0;
	if(value == max||value == min)
	{
		order = 10000;
	}
	pos++;
	
	return order;
}


