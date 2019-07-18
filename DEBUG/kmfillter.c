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

float filter(float z)
{
	X_pre = A*Xkf_k1;
	P_pre = P_k1+Q;
	Kg = P_pre/(P_pre+R);
	Xkf_k1 = X_pre+Kg*(z-C*X_pre);
	P_k1 = (1-Kg*C)*P_pre;
	return Xkf_k1;
}