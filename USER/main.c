#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO    1           
#define START_STK_SIZE     128      
TaskHandle_t StartTaskHanhler;       
void StartTask(void *pvParameters);    

#define LED0_TASK_PRIO    2       
#define LED0_STK_SIZE     50         
TaskHandle_t LED0TaskHanhler;       
void LED0Task(void *pArg);           

#define LED1_TASK_PRIO    3         
#define LED1_STK_SIZE     50       
TaskHandle_t LED1TaskHanhler;        
void LED1Task(void *pArg);         



int main(void)
{
	u8 len;	
	u16 times=0; 
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();				              //初始化HAL库
	Stm32_Clock_Init(160,5,2,4);    //设置时钟,400Mhz 
	delay_init(400);				        //延时初始化
	uart_init(115200);			      	//串口初始化
	LED_Init();
	
	printf("System start\r\n");
	
	xTaskCreate( (TaskFunction_t) StartTask,       //Task Function
						 (const char*   ) "StartTask",     //Task Name
						 (uint16_t      ) START_STK_SIZE,  //Task Heap size
						 (void *        ) NULL,            //Task parameter
						 (UBaseType_t   ) START_TASK_PRIO, //Task properity
						 (TaskHandle_t* ) &StartTaskHanhler//Task Handler
						);
  vTaskStartScheduler();
					 }
							 
void StartTask(void * pvParameter)
{
	taskENTER_CRITICAL();     //进入临界区
  xTaskCreate( (TaskFunction_t) LED0Task,        //任务函数
							 (const char*   ) "LED0Task",      //任务名称
							 (uint16_t      ) LED0_STK_SIZE,   //任务堆栈大小
							 (void *        ) NULL,            //传递给任务的参数
							 (UBaseType_t   ) LED0_TASK_PRIO,  //任务优先级
							 (TaskHandle_t* ) &LED0TaskHanhler //任务句柄
	            );
	
	xTaskCreate( (TaskFunction_t) LED1Task,        //任务函数
							 (const char*   ) "LED1Task",      //任务名称
						 (uint16_t      ) LED1_STK_SIZE,     //任务堆栈大小
						 (void *        ) NULL,              //传递给任务函数的参数
						 (UBaseType_t   ) LED1_TASK_PRIO,    //任务优先级
						 (TaskHandle_t* ) &LED1TaskHanhler   //任务句柄
						);
	vTaskDelete(StartTaskHanhler);
	  
  taskEXIT_CRITICAL();      //退出临界区
}

void LED0Task(void *pArg)
{
	static int i =0;
	while(1)
	{
		printf("hell1\r\n");
		LED0_Toggle;
		vTaskDelay(500);
		
	}
}
void LED1Task(void *pArg)
{
	while(1)
	{
		
		printf("hello\r\n");
		LED1(0);
		vTaskDelay(200);
		LED1(1);
		vTaskDelay(800);
		
	}
}

