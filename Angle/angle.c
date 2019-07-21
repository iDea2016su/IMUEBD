#include "angle.h"
#include "math.h"

static float systemAngle;

double Re[9] = {0};
double normsqA;

#define g  9.80
#define PI 3.1425926
double freeFallGravitySquared = 0.01 * g * g;

float Xoffset = 166.677;
float Yoffset = 32.48;
float Zoffset = -150.075;	

u8 getRotationMatrix(double gravity[], double geomagnetic[]) 
{
		// TODO: move this to native code for efficiency
		double Ax = gravity[0];
		double Ay = gravity[1];
		double Az = gravity[2];

		normsqA = (Ax * Ax + Ay * Ay + Az * Az);
		
		
		if (normsqA < freeFallGravitySquared) {
				// gravity less than 10% of normal value
			printf("gravity less than 10 of normal value\r\n");
				return false;
		}
		double Ex = geomagnetic[0];
		double Ey = geomagnetic[1];
		double Ez = geomagnetic[2];
		double Hx = Ey * Az - Ez * Ay;
		double Hy = Ez * Ax - Ex * Az;
		double Hz = Ex * Ay - Ey * Ax;
		double normH = (double)sqrt(Hx * Hx + Hy * Hy + Hz * Hz);
		if (normH < 0.1f) {
				// device is close to free fall (or in space?), or close to
				// magnetic north pole. Typical values are  > 100.
				printf("magnetic less than 10 of normal value\r\n");
				return false;
		}
		double invH = 1.0 / normH;
		Hx *= invH;
		Hy *= invH;
		Hz *= invH;
		double invA = 1.0 / (double) sqrt(Ax * Ax + Ay * Ay + Az * Az);
		Ax *= invA;
		Ay *= invA;
		Az *= invA;
		double Mx = Ay * Hz - Az * Hy;
		double My = Az * Hx - Ax * Hz;
		double Mz = Ax * Hy - Ay * Hx;

		Re[0] = Hx;     Re[1] = Hy;     Re[2] = Hz;
		Re[3] = Mx;     Re[4] = My;     Re[5] = Mz;
		Re[6] = Ax;     Re[7] = Ay;     Re[8] = Az;
				
		return true;
}

void getOrientation(double gravity[], double geomagnetic[]) 
{
	  double acc[3] = {0};
		double m[3] = {0};
		double angle[3] = {0};
		acc[0] = gravity[0]/100.0f;
		acc[1] = gravity[1]/100.0f;
		acc[2] = gravity[2]/100.0f;
		m[0] = (geomagnetic[0] - Xoffset);
		m[1] = (geomagnetic[1] - Yoffset);
		m[2] = (geomagnetic[2] - Zoffset);
	  getRotationMatrix(acc,m);
		angle[0] = (float) atan2(Re[1], Re[4]);
		angle[1] = (float) asin(-Re[7]);
		angle[2] = (float) atan2(-Re[6], Re[8]);
	  printf("x:%d\r\n",(int)getAngle360(angle[0]));
	  //printf("angle %5.3f\r\n",getInclination()*180/PI);
}

double getAngle360(double anglePi)
{
	return anglePi*180/PI;
}

/********************************************/
float Data_conversion(float *AccBuffer,float *MagBuffer)
{
  unsigned char i;
	float HeadingValue = 0.0f;
	float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f;
  float fTiltedX,fTiltedY = 0.0f;
	float fcosf=0;
	for(i=0;i<3;i++)
	{
		AccBuffer[i] /= 100.0f;
	}
  fNormAcc = sqrt((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));
      
	fSinRoll = AccBuffer[1]/fNormAcc;
	fCosRoll = sqrt(1.0-(fSinRoll * fSinRoll));
	fSinPitch = AccBuffer[0]/fNormAcc;
	fCosPitch = sqrt(1.0-(fSinPitch * fSinPitch));

	MagBuffer[0] = MagBuffer[0] - Xoffset;
	MagBuffer[1] = MagBuffer[1] - Yoffset;
	MagBuffer[2] = MagBuffer[2] - Zoffset;
  
	fTiltedX = MagBuffer[0]*fCosPitch + MagBuffer[2]*fSinPitch;
	fTiltedY = MagBuffer[0]*fSinRoll*fSinPitch + MagBuffer[1]*fCosRoll - MagBuffer[2]*fSinRoll*fCosPitch;
			
	fcosf=fTiltedX /sqrt(fTiltedX*fTiltedX+fTiltedY*fTiltedY);
	
	if(fTiltedY>0)
		HeadingValue = (float)(acos(fcosf)*180/PI);
	else
		HeadingValue =360-(float)(acos(fcosf)*180/PI);
			
      //HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;
      HeadingValue+=11;//
			if(HeadingValue>360)
			{
				HeadingValue=HeadingValue-360;
			}
   
	    return HeadingValue ;
}

void setAngle(float a)
{
	systemAngle = a;
}
float getAngle(void)
{
	return systemAngle;
}