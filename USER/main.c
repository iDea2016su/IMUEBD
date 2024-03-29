#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include "lsm9ds1_reg.h"
#include "myiic.h"
#include "i2c.h"

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t data_raw_magnetic_field;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float magnetic_field_mgauss[3];
static lsm9ds1_id_t whoamI;
static lsm9ds1_status_t reg;
static uint8_t rst;
static uint8_t tx_buffer[1000];

/* Private functions ---------------------------------------------------------*/
/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
void platform_init(void);



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


/* Initialize inertial sensors (IMU) driver interface */
  uint8_t i2c_add_mag = LSM9DS1_MAG_I2C_ADD_L;
  lsm9ds1_ctx_t dev_ctx_mag;

  /* Initialize magnetic sensors driver interface */
  uint8_t i2c_add_imu = LSM9DS1_IMU_I2C_ADD_H;
  lsm9ds1_ctx_t dev_ctx_imu;

	

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
	
	platform_init();
	

	
  dev_ctx_mag.write_reg = platform_write;
  dev_ctx_mag.read_reg = platform_read;
  dev_ctx_mag.handle = (void*)&i2c_add_mag;
	
  dev_ctx_imu.write_reg = platform_write;
  dev_ctx_imu.read_reg = platform_read;
  dev_ctx_imu.handle = (void*)&i2c_add_imu;

	

  /* Check device ID */
  lsm9ds1_dev_id_get(&dev_ctx_mag, &dev_ctx_imu, &whoamI);
  if (whoamI.imu != LSM9DS1_IMU_ID || whoamI.mag != LSM9DS1_MAG_ID){
    while(1){
      /* manage here device not found */
    }
		
  }
	printf("System start1\r\n");
  /* Restore default configuration */
  lsm9ds1_dev_reset_set(&dev_ctx_mag, &dev_ctx_imu, PROPERTY_ENABLE);
  do {
    lsm9ds1_dev_reset_get(&dev_ctx_mag, &dev_ctx_imu, &rst);
  } while (rst);

  /* Enable Block Data Update */
  lsm9ds1_block_data_update_set(&dev_ctx_mag, &dev_ctx_imu, PROPERTY_ENABLE);

  /* Set full scale */
  lsm9ds1_xl_full_scale_set(&dev_ctx_imu, LSM9DS1_4g);
  lsm9ds1_gy_full_scale_set(&dev_ctx_imu, LSM9DS1_2000dps);
  lsm9ds1_mag_full_scale_set(&dev_ctx_mag, LSM9DS1_16Ga);

  /* Configure filtering chain - See datasheet for filtering chain details */
  /* Accelerometer filtering chain */
  lsm9ds1_xl_filter_aalias_bandwidth_set(&dev_ctx_imu, LSM9DS1_AUTO);
  lsm9ds1_xl_filter_lp_bandwidth_set(&dev_ctx_imu, LSM9DS1_LP_ODR_DIV_50);
  lsm9ds1_xl_filter_out_path_set(&dev_ctx_imu, LSM9DS1_LP_OUT);
  /* Gyroscope filtering chain */
  lsm9ds1_gy_filter_lp_bandwidth_set(&dev_ctx_imu, LSM9DS1_LP_ULTRA_LIGHT);
  lsm9ds1_gy_filter_hp_bandwidth_set(&dev_ctx_imu, LSM9DS1_HP_MEDIUM);
  lsm9ds1_gy_filter_out_path_set(&dev_ctx_imu, LSM9DS1_LPF1_HPF_LPF2_OUT);

  /* Set Output Data Rate / Power mode */
  lsm9ds1_imu_data_rate_set(&dev_ctx_imu, LSM9DS1_IMU_59Hz5);
  lsm9ds1_mag_data_rate_set(&dev_ctx_mag, LSM9DS1_MAG_UHP_10Hz);

  /* Read samples in polling mode (no int) */
  while(1)
  {
    /* Read device status register */
    lsm9ds1_dev_status_get(&dev_ctx_mag, &dev_ctx_imu, &reg);

    if ( reg.status_imu.xlda && reg.status_imu.gda )
    {
      /* Read imu data */
      memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
      memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));

      lsm9ds1_acceleration_raw_get(&dev_ctx_imu, data_raw_acceleration.u8bit);
      lsm9ds1_angular_rate_raw_get(&dev_ctx_imu, data_raw_angular_rate.u8bit);

      acceleration_mg[0] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[0]);
      acceleration_mg[1] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[1]);
      acceleration_mg[2] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[2]);

      angular_rate_mdps[0] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
      angular_rate_mdps[1] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
      angular_rate_mdps[2] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);

      sprintf((char*)tx_buffer, "IMU - [mg]:%4.2f\t%4.2f\t%4.2f\t[mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
              acceleration_mg[0], acceleration_mg[1], acceleration_mg[2],
              angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
      //tx_com(tx_buffer, strlen((char const*)tx_buffer));
			printf("%s",tx_buffer);
    }

    if ( reg.status_mag.zyxda )
    {
      /* Read magnetometer data */
      memset(data_raw_magnetic_field.u8bit, 0x00, 3 * sizeof(int16_t));

      lsm9ds1_magnetic_raw_get(&dev_ctx_mag, data_raw_magnetic_field.u8bit);

      magnetic_field_mgauss[0] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[0]);
      magnetic_field_mgauss[1] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[1]);
      magnetic_field_mgauss[2] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[2]);

      sprintf((char*)tx_buffer, "MAG - [mG]:%4.2f\t%4.2f\t%4.2f\r\n",
              magnetic_field_mgauss[0], magnetic_field_mgauss[1], magnetic_field_mgauss[2]);
      printf("%s",tx_buffer);
    }
  }
}
	
//	xTaskCreate( (TaskFunction_t) StartTask,       //Task Function
//						 (const char*   ) "StartTask",     //Task Name
//						 (uint16_t      ) START_STK_SIZE,  //Task Heap size
//						 (void *        ) NULL,            //Task parameter
//						 (UBaseType_t   ) START_TASK_PRIO, //Task properity
//						 (TaskHandle_t* ) &StartTaskHanhler//Task Handler
//						);
//  vTaskStartScheduler();
//					 }
							 
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

int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
  uint8_t *i2c_address = handle;
  HAL_I2C_Mem_Write(&hi2c1, *i2c_address, reg,I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor I2C address.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
  uint8_t *i2c_address = handle;
  HAL_I2C_Mem_Read(&hi2c1, *i2c_address, reg,I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  return 0;
}

///*
// * @brief  Write generic device register (platform dependent)
// *
// * @param  tx_buffer     buffer to trasmit
// * @param  len           number of byte to send
// *
// */
//static void tx_com(uint8_t *tx_buffer, uint16_t len)
//{
//  
//}

/*
 * @brief  platform specific initialization (platform dependent)
 */
void platform_init(void)
{ 
  MX_I2C1_Init();
	//HAL_Delay(1000);
}