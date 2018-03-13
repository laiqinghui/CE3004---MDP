#include "Constants.h"
#include "PID.h"
#include <EEPROM.h>
//----------------------------------------------------------------TURNING----------------------------------------------------------------
double getTurnValueOffset(int dir, int turnDegree);
void setTurnValueOffset(int dir, int turnDegree, double newValue);
double getTurnAmount(int dir, int turnDegree);
void turnPID(int dir, int degree);

double turnRight90Offset = 0;
double turnLeft90Offset = 0;

double getTurnValueOffset(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
	return turnRight90Offset;
  }
  //Left Turn
  else
  {
    return turnLeft90Offset;
  }
}

void setTurnValueOffset(int dir, int turnDegree, double newValue){
  //Right Turn
  if (dir == 1)
  {
		turnRight90Offset = newValue;
    EEPROM.write(0, (newValue*100)>>8);
    EEPROM.write(1, (newValue*100)&0xFF);
  }
  //Left Turn
  else
  {
     turnLeft90Offset = newValue;
     EEPROM.write(2, (newValue*100)>>8);
     EEPROM.write(3, (newValue*100)&0xFF);
  }
}

double getTurnAmount(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    //180
    if (turnDegree == 180)
    {
	    return abs(47.7 * 0.5 * ticksPerCM);  
    }
    //90
    else
    {
		return abs(44.52 * (turnDegree / 360.0) * ticksPerCM) - turnRight90Offset;
      
    }
  }
  //Left Turn
  else
  {
    return abs(43.25 * (turnDegree / 360.0) * ticksPerCM) - turnLeft90Offset;
  }
}

int m1CurrentWidthPositive = 955;
int m1CurrentWidthNegative = 975;
int m2TurnSpeedPostive = 330;
int m2TurnSpeedNegative = -320;

void turnPID(int dir, int degree){
	sideWall[0] = 0;
	sideWall[1] = 0;
	sideWall[2] = 0;
	
	int amount = getTurnAmount(dir, degree);
	
	int total = 0;
	int currentTicksM1 = 0;
	int currentM2Width = 0;
	int difference = 0;
	
	enableInterrupt( e1a, risingM1Ticks, RISING);
	enableInterrupt( e2b, risingM2, RISING);
	setTicks(0, 0);
	
	if(dir == 1)
	{
		int m1Speed = -310;
		int m2Speed = 330;
		md.setSpeeds(m1Speed, m2Speed);
		
		while(currentTicksM1 < amount)
		{
			
			noInterrupts();
			currentTicksM1 = M1ticks;
			currentM2Width = squareWidth_M2;
			interrupts();
     
			if(currentM2Width > m1CurrentWidthNegative)
			{
				m2Speed = m2Speed - 1;
				OCR1B = m2Speed;
			}
			else if(currentM2Width < m1CurrentWidthNegative)
			{
				m2Speed = m2Speed + 1;
				OCR1B = m2Speed;
			}
		}
	}
	else
	{
		int m1Speed = 310;
		int m2Speed = -320;
		md.setSpeeds(m1Speed, m2Speed);

		while(currentTicksM1 < amount)
		{
			noInterrupts();
			currentTicksM1 = M1ticks;
			currentM2Width = squareWidth_M2;
			interrupts();
     
			if(currentM2Width < m1CurrentWidthPositive)
			{
				m2Speed = m2Speed - 1;
				OCR1B = m2Speed;
			}
			else if(currentM2Width > m1CurrentWidthPositive)
			{
				m2Speed = m2Speed + 1;
				OCR1B = m2Speed;
			}
		}
	}
	md.setBrakes(400, 400);

	disableInterrupt(e1a);
	disableInterrupt(e2b);
	setTicks(0, 0);
	setSqWidth(0,0);
}
