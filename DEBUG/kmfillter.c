#include "kmfillter.h"

static float Q = 0.01;  //过程噪声方差
static float R = 10;  //观测噪声方差
static float initial = 0; //初值
static float A = 1;       //状态系数
static float C = 1;       //观测系数
static float X_pre = 0; 
static float P_pre = 0;
static float Xkf_k1 = 0;
static float P_k1 = 1;
static float Kg = 0;

static float StepXQ = 0.01;  //过程噪声方差
static float StepXR = 10;  //观测噪声方差
static float StepXinitial = 0; //初值
static float StepXA = 1;       //状态系数
static float StepXC = 1;       //观测系数
static float StepXX_pre = 0; 
static float StepXP_pre = 0;
static float StepXXkf_k1 = 0;
static float StepXP_k1 = 1;
static float StepXKg = 0;

static float StepYQ = 0.01;  //过程噪声方差
static float StepYR = 10;  //观测噪声方差
static float StepYinitial = 0; //初值
static float StepYA = 1;       //状态系数
static float StepYC = 1;       //观测系数
static float StepYX_pre = 0; 
static float StepYP_pre = 0;
static float StepYXkf_k1 = 0;
static float StepYP_k1 = 1;
static float StepYKg = 0;

static float StepZQ = 0.01;  //过程噪声方差
static float StepZR = 10;  //观测噪声方差
static float StepZinitial = 0; //初值
static float StepZA = 1;       //状态系数
static float StepZC = 1;       //观测系数
static float StepZX_pre = 0; 
static float StepZP_pre = 0;
static float StepZXkf_k1 = 0;
static float StepZP_k1 = 1;
static float StepZKg = 0;


static float AngleXQ = 0.1;  //过程噪声方差
static float AngleXR = 0.1;  //观测噪声方差
static float AngleXinitial = 0; //初值
static float AngleXA = 1;       //状态系数
static float AngleXC = 1;       //观测系数
static float AngleXX_pre = 0; 
static float AngleXP_pre = 0;
static float AngleXXkf_k1 = 0;
static float AngleXP_k1 = 1;
static float AngleXKg = 0;

static float AngleYQ = 0.1;  //过程噪声方差
static float AngleYR = 0.1;  //观测噪声方差
static float AngleYinitial = 0; //初值
static float AngleYA = 1;       //状态系数
static float AngleYC = 1;       //观测系数
static float AngleYX_pre = 0; 
static float AngleYP_pre = 0;
static float AngleYXkf_k1 = 0;
static float AngleYP_k1 = 1;
static float AngleYKg = 0;

static float AngleZQ = 0.1;  //过程噪声方差
static float AngleZR = 0.1;  //观测噪声方差
static float AngleZinitial = 0; //初值
static float AngleZA = 1;       //状态系数
static float AngleZC = 1;       //观测系数
static float AngleZX_pre = 0; 
static float AngleZP_pre = 0;
static float AngleZXkf_k1 = 0;
static float AngleZP_k1 = 1;
static float AngleZKg = 0;

static float MXQ = 0.1;  //过程噪声方差
static float MXR = 0.1;  //观测噪声方差
static float MXinitial = 0; //初值
static float MXA = 1;       //状态系数
static float MXC = 1;       //观测系数
static float MXX_pre = 0; 
static float MXP_pre = 0;
static float MXXkf_k1 = 0;
static float MXP_k1 = 1;
static float MXKg = 0;

static float MYQ = 0.1;  //过程噪声方差
static float MYR = 0.1;  //观测噪声方差
static float MYinitial = 0; //初值
static float MYA = 1;       //状态系数
static float MYC = 1;       //观测系数
static float MYX_pre = 0; 
static float MYP_pre = 0;
static float MYXkf_k1 = 0;
static float MYP_k1 = 1;
static float MYKg = 0;

static float MZQ = 0.1;  //过程噪声方差
static float MZR = 0.1;  //观测噪声方差
static float MZinitial = 0; //初值
static float MZA = 1;       //状态系数
static float MZC = 1;       //观测系数
static float MZX_pre = 0; 
static float MZP_pre = 0;
static float MZXkf_k1 = 0;
static float MZP_k1 = 1;
static float MZKg = 0;

float filter(float z)
{
	X_pre = A*Xkf_k1;
	P_pre = P_k1+Q;
	Kg = P_pre/(P_pre+R);
	Xkf_k1 = X_pre+Kg*(z-C*X_pre);
	P_k1 = (1-Kg*C)*P_pre;
	return Xkf_k1;
}

float filterStepX(float z)
{
	StepXX_pre = StepXA*StepXXkf_k1;
	StepXP_pre = StepXP_k1+StepXQ;
	StepXKg = StepXP_pre/(StepXP_pre+StepXR);
	StepXXkf_k1 = StepXX_pre+StepXKg*(z-StepXC*StepXX_pre);
	StepXP_k1 = (1-StepXKg*StepXC)*StepXP_pre;
	return StepXXkf_k1;
}

float filterStepY(float z)
{
	StepYX_pre = StepYA*StepYXkf_k1;
	StepYP_pre = StepYP_k1+StepYQ;
	StepYKg = StepYP_pre/(StepYP_pre+StepYR);
	StepYXkf_k1 = StepYX_pre+StepYKg*(z-StepYC*StepYX_pre);
	StepYP_k1 = (1-StepYKg*StepYC)*StepYP_pre;
	return StepYXkf_k1;
}
float filterStepZ(float z)
{
	StepZX_pre = StepZA*StepZXkf_k1;
	StepZP_pre = StepZP_k1+StepZQ;
	StepZKg = StepZP_pre/(StepZP_pre+StepZR);
	StepZXkf_k1 = StepZX_pre+StepZKg*(z-StepZC*StepZX_pre);
	StepZP_k1 = (1-StepZKg*StepZC)*StepZP_pre;
	return StepZXkf_k1;
}

float filterAngleX(float z)
{
	AngleXX_pre = AngleXA*AngleXXkf_k1;
	AngleXP_pre = AngleXP_k1+AngleXQ;
	AngleXKg = AngleXP_pre/(AngleXP_pre+AngleXR);
	AngleXXkf_k1 = AngleXX_pre+AngleXKg*(z-AngleXC*AngleXX_pre);
	AngleXP_k1 = (1-AngleXKg*AngleXC)*AngleXP_pre;
	return AngleXXkf_k1;
}

float filterAngleY(float z)
{
	AngleYX_pre = AngleYA*AngleYXkf_k1;
	AngleYP_pre = AngleYP_k1+AngleYQ;
	AngleYKg = AngleYP_pre/(AngleYP_pre+AngleYR);
	AngleYXkf_k1 = AngleYX_pre+AngleYKg*(z-AngleYC*AngleYX_pre);
	AngleYP_k1 = (1-AngleYKg*AngleYC)*AngleYP_pre;
	return AngleYXkf_k1;
}
float filterAngleZ(float z)
{
	AngleZX_pre = AngleZA*AngleZXkf_k1;
	AngleZP_pre = AngleZP_k1+AngleZQ;
	AngleZKg = AngleZP_pre/(AngleZP_pre+AngleZR);
	AngleZXkf_k1 = AngleZX_pre+AngleZKg*(z-AngleZC*AngleZX_pre);
	AngleZP_k1 = (1-AngleZKg*AngleZC)*AngleZP_pre;
	return AngleZXkf_k1;
}