#include "kmfillter.h"

static float AQ = 0.01;  //过程噪声方差
static float AR = 10;  //观测噪声方差
static float Ainitial = 0; //初值
static float AA = 1;       //状态系数
static float AC = 1;       //观测系数
static float AX_pre = 0; 
static float AP_pre = 0;
static float AXkf_k1 = 0;
static float AP_k1 = 1;
static float AKg = 0;

static float BQ = 0.01;  //过程噪声方差
static float BR = 10;  //观测噪声方差
static float Binitial = 0; //初值
static float BA = 1;       //状态系数
static float BC = 1;       //观测系数
static float BX_pre = 0; 
static float BP_pre = 0;
static float BXkf_k1 = 0;
static float BP_k1 = 1;
static float BKg = 0;

static float CQ = 0.01;  //过程噪声方差
static float CR = 10;  //观测噪声方差
static float Cinitial = 0; //初值
static float CA = 1;       //状态系数
static float CC = 1;       //观测系数
static float CX_pre = 0; 
static float CP_pre = 0;
static float CXkf_k1 = 0;
static float CP_k1 = 1;
static float CKg = 0;

float filterA(float Az)
{
	AX_pre = AA*AXkf_k1;
	AP_pre = AP_k1+AQ;
	AKg = AP_pre/(AP_pre+AR);
	AXkf_k1 = AX_pre+AKg*(Az-AC*AX_pre);
	AP_k1 = (1-AKg*AC)*AP_pre;
	return AXkf_k1;
}

float filterB(float Bz)
{
	BX_pre = BA*BXkf_k1;
	BP_pre = BP_k1+BQ;
	BKg = BP_pre/(BP_pre+BR);
	BXkf_k1 = BX_pre+BKg*(Bz-BC*BX_pre);
	BP_k1 = (1-BKg*BC)*BP_pre;
	return BXkf_k1;
}

float filterC(float Cz)
{
	CX_pre = CA*CXkf_k1;
	CP_pre = CP_k1+CQ;
	CKg = CP_pre/(CP_pre+CR);
	CXkf_k1 = CX_pre+CKg*(Cz-CC*CX_pre);
	CP_k1 = (1-CKg*CC)*CP_pre;
	return AXkf_k1;
}