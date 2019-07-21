#include "angle.h"
#include "math.h"

static float Re[9] = {0};
static float I[9] = {0};
static float accelerometerValues[3] = {0};
static float magneticFieldValues[3] = {0};
static float normsqA;
static float values[3]={0};

#define g  9.80
#define DECLINATION -8.58
#define PI 3.1425926
static float freeFallGravitySquared = 0.01f * g * g;

u8 getRotationMatrix(float gravity[], float geomagnetic[]) 
{
		// TODO: move this to native code for efficiency
		float Ax = gravity[0]/100;
		float Ay = gravity[1]/100;
		float Az = gravity[2]/100;

		normsqA = (Ax * Ax + Ay * Ay + Az * Az);
		
		
		if (normsqA < freeFallGravitySquared) {
				// gravity less than 10% of normal value
			printf("gravity less than 10 of normal value\r\n");
			while(1);
				return false;
		}
		float Ex = geomagnetic[0];
		float Ey = geomagnetic[1];
		float Ez = geomagnetic[2];
		float Hx = Ey * Az - Ez * Ay;
		float Hy = Ez * Ax - Ex * Az;
		float Hz = Ex * Ay - Ey * Ax;
		float normH = (float)sqrt(Hx * Hx + Hy * Hy + Hz * Hz);
		if (normH < 0.1f) {
				// device is close to free fall (or in space?), or close to
				// magnetic north pole. Typical values are  > 100.
				printf("magnetic less than 10 of normal value\r\n");
			  while(1);
				return false;
		}
		float invH = 1.0f / normH;
		Hx *= invH;
		Hy *= invH;
		Hz *= invH;
		float invA = 1.0f / (float) sqrt(Ax * Ax + Ay * Ay + Az * Az);
		Ax *= invA;
		Ay *= invA;
		Az *= invA;
		float Mx = Ay * Hz - Az * Hy;
		float My = Az * Hx - Ax * Hz;
		float Mz = Ax * Hy - Ay * Hx;

		Re[0] = Hx;     Re[1] = Hy;     Re[2] = Hz;
		Re[3] = Mx;     Re[4] = My;     Re[5] = Mz;
		Re[6] = Ax;     Re[7] = Ay;     Re[8] = Az;
				
		// compute the inclination matrix by projecting the geomagnetic
		// vector onto the Z (gravity) and X (horizontal component
		// of geomagnetic vector) axes.
		float invE = 1.0f / (float)sqrt(Ex * Ex + Ey * Ey + Ez * Ez);
		float c = (Ex * Mx + Ey * My + Ez * Mz) * invE;
		float s = (Ex * Ax + Ey * Ay + Ez * Az) * invE;
		I[0] = 1;     I[1] = 0;     I[2] = 0;
		I[3] = 0;     I[4] = c;     I[5] = s;
		I[6] = 0;     I[7] = -s;     I[8] = c;
		return true;
}

void getOrientation(float gravity[], float geomagnetic[], float angle[]) 
{
	  getRotationMatrix(gravity,geomagnetic);
		angle[0] = (float) atan2(Re[1], Re[4]);
		angle[1] = (float) asin(-Re[7]);
		angle[2] = (float) atan2(-Re[6], Re[8]);
	  //printf("x:%d y:%d z:%d\r\n",(int)getAngle360(angle[0]),(int)getAngle360(angle[1]),(int)getAngle360(angle[2]));
	  //printf("angle %5.3f\r\n",getInclination()*180/PI);
}

float getInclination() 
{
    return (float) atan2(I[5], I[4]);
}
float getAngle360(float anglePi)
{
	return anglePi*180/PI;
}

void printAttitude(float a[], float m[])
{
  float roll = atan2(a[1], a[2]);
  float pitch = atan2(-a[0], sqrt(a[1] * a[1] + a[2] * a[2]));
  
  float heading;
  if (m[1] == 0)
    heading = (m[0] < 0) ? 180.0 : 0;
  else
    heading = atan2(m[0], m[1]);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;                     
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  //printf("x:%d y:%d z:%d\r\n",(int)roll,(int)pitch,(int)heading);
}