#include "Constants.h"
#include "PID.h"
//----------------------------------------------------------------TURNING----------------------------------------------------------------
double getDiameterValue(int dir, int turnDegree);
void setDiameterValue(int dir, int turnDegree, double newValue);
double getTurnAmount(int dir, int turnDegree);
void turnPID(int dir, int degree);

double turnRight90 = 44.52;
double turnRight180 = 47.7;
double turnLeft90 = 43.25;

double getDiameterValue(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    if (turnDegree == 180)
    {
		return turnRight180;
    }
    else
    {
		return turnRight90;
    }
  }
  //Left Turn
  else
  {
    return turnLeft90;
  }
}

void setDiameterValue(int dir, int turnDegree, double newValue){
	Serial.println("Set");
	Serial.println(getTurnAmount(dir,turnDegree));
  //Right Turn
  if (dir == 1)
  {
    //180
    if (turnDegree == 180)
    {
	  turnRight180 = newValue;
      
    }
    //90
    else
    {
		turnRight90 = newValue;
      
    }
  }
  //Left Turn
  else
  {
     turnLeft90 = newValue;
  }
  Serial.println(newValue);
}

double getTurnAmount(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    //180
    if (turnDegree == 180)
    {
	  return abs(turnRight180 * 0.5 * ticksPerCM);
      
    }
    //90
    else
    {
		return abs(turnRight90 * (turnDegree / 360.0) * ticksPerCM);
      
    }
  }
  //Left Turn
  else
  {
    return abs(turnLeft90 * (turnDegree / 360.0) * ticksPerCM);
  }
}

void turnPID(int dir, int degree){
	sideWall[0] = 0;
	sideWall[1] = 0;
	sideWall[2] = 0;
	
	int amount = getTurnAmount(dir, degree);
	
	int total = 0;
	int currentTicksM1 = 0;
	int currentTicksM2 = 0;
	int difference = 0;
	
	enableInterrupt( e1a, risingM1Ticks, RISING);
	enableInterrupt( e2b, risingM2Ticks, RISING);
	setTicks(0, 0);
	
	if(dir == 1)
	{
		int m1Speed = -302;
		int m2Speed = 330;
		md.setSpeeds(m1Speed, m2Speed);
		
		while(currentTicksM1 < amount)
		{
			
			noInterrupts();
			currentTicksM1 = M1ticks;
			difference = M1ticks - M2ticks;
			interrupts();
     
			if(difference < 0)
			{
				m2Speed = m2Speed - 1;
				OCR1B = m2Speed;
			}
			else if(difference > 0)
			{
				m2Speed = m2Speed + 1;
				OCR1B = m2Speed;
			}
		}
	}
	else
	{
		int m1Speed = 302;
		int m2Speed = -330;
		md.setSpeeds(m1Speed, m2Speed);

		while(currentTicksM1 < amount)
		{
			noInterrupts();
			currentTicksM1 = M1ticks;
			difference = M1ticks - M2ticks;
			interrupts();
     
			if(difference < 0)
			{
				m2Speed = m2Speed + 1;
				OCR1B = m2Speed;
			}
			else if(difference > 0)
			{
				m2Speed = m2Speed - 1;
				OCR1B = m2Speed;
			}
		}
	}
	md.setBrakes(400, 400);

	disableInterrupt(e1a);
	disableInterrupt(e2b);
	setTicks(0, 0);
}
