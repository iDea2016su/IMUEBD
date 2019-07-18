#include "scope.h"
#include "usart.h"

float OutData[4] = {0};
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;
    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}

void send(float ch1,float ch2,float ch3,float ch4)
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  unsigned char i;
  unsigned short CRC16 = 0;

  temp[0]  = (int)ch1;
  temp1[0] = (unsigned int)temp[0];
  temp[1]  = (int)ch2;
  temp1[1] = (unsigned int)temp[1];
  temp[2]  = (int)ch3;
  temp1[2] = (unsigned int)temp[2];
  temp[3]  = (int)ch4;
  temp1[3] = (unsigned int)temp[3];
  for(i=0;i<4;i++) 
  {
    databuf[i*2]   = (unsigned char)(temp1[i]%256);
    databuf[i*2+1] = (unsigned char)(temp1[i]/256);
  }
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  for(i=0;i<10;i++)
	{
		while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	  USART1->TDR=(u8)databuf[i];    
	} 
}