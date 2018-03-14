#include "Constants.h"
#include "PID.h"
#include <EEPROM.h>
//----------------------------------------------------------------TURNING----------------------------------------------------------------
double getTurnValueOffset(int dir);
void setTurnValueOffset(int dir, double newValue);
double getTurnAmount(int dir, int turnDegree);
void turnPID(int dir, int degree);

double turnRight90Offset = 0;
double turnLeft90Offset = 0;

double getTurnValueOffset(int dir) {
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

void setTurnValueOffset(int dir, double newValue) {
  //Right Turn
  if (dir == 1)
  {
    turnRight90Offset = newValue;
    EEPROM.write(0, ((int)(newValue * 100)) >> 8);
    EEPROM.write(1, ((int)(newValue * 100)) & 0xFF);
  }
  //Left Turn
  else
  {
    turnLeft90Offset = newValue;
    EEPROM.write(2, ((int)(newValue * 100)) >> 8);
    EEPROM.write(3, ((int)(newValue * 100)) & 0xFF);
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
int m2TurnSpeedPositive = 330;
int m2TurnSpeedNegative = -320;

void turnTest(int dir, int degree) {
  
  sideWall[0] = 0;
  sideWall[1] = 0;
  sideWall[2] = 0;
    
  int amount = getTurnAmount(dir, degree);

  int currentTicksM1 = 0;
  int currentTicksM2 = 0;
  int currentM2Width = 0;
  int currentTime = 0;
  int prevTime = 0;
  int prevM2Width = 0;

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);
  setTicks(0, 0);
  
  if (dir == 1)
  {
    int m1Speed = -310;
    md.setSpeeds(m1Speed, m2TurnSpeedPositive);
	
    while (currentTicksM1 < amount)
    {

      noInterrupts();
      currentTicksM1 = M1ticks;
	  currentTicksM2 = M2ticks;
      interrupts();
	  
	  if(currentTicksM1 > currentTicksM2)
	  {
		  m2TurnSpeedPositive = m2TurnSpeedPositive + 1;
	  }
	  else if(currentTicksM1 < currentTicksM2)
	  {
		  m2TurnSpeedPositive = m2TurnSpeedPositive - 1;
	  }
	  
	  /*
	  currentTime = micros();
	  if(currentTime-prevTime == 3000)
	  {
		  //Serial.println(m2TurnSpeedPositive);
		  //Serial.println(currentM2Width);
		  //Serial.println(m1CurrentWidthNegative);
		  if (currentM2Width > m1CurrentWidthNegative)
		  {
			m2TurnSpeedPositive = m2TurnSpeedPositive + 1;
			OCR1B = m2TurnSpeedPositive;
		  }
		  else if (currentM2Width < m1CurrentWidthNegative)
		  {
			m2TurnSpeedPositive = m2TurnSpeedPositive - 1;
			OCR1B = m2TurnSpeedPositive;
		  }
		  prevTime = currentTime;
	  }
		*/	  
    }
  }
  else
  {
    int m1Speed = 310;
    md.setSpeeds(m1Speed, m2TurnSpeedNegative);

    while (currentTicksM1 < amount)
    {
	  noInterrupts();
      currentTicksM1 = M1ticks;
	  currentTicksM2 = M2ticks;
      interrupts();
	  
	  if(currentTicksM1 > currentTicksM2)
	  {
		  m2TurnSpeedNegative = m2TurnSpeedNegative - 1;
	  }
	  else if(currentTicksM1 < currentTicksM2)
	  {
		  m2TurnSpeedNegative = m2TurnSpeedNegative + 1;
	  }
	  
    }
  }
  md.setBrakes(400, 400);

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);

}

void turnPID(int dir, int degree) {
  
  sideWall[0] = 0;
  sideWall[1] = 0;
  sideWall[2] = 0;
  
  //Serial.println("Set");
  //Serial.println(m2TurnSpeedNegative);
  //Serial.println(m2TurnSpeedPositive);
  
  int amount = getTurnAmount(dir, degree);

  int currentTicksM1 = 0;
  int currentM1Width =0;
  int currentM2Width = 0;
  int currentTime = 0;
  int prevTime = 0;
  int prevM2Width = 0;

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);
  setTicks(0, 0);
  
  if (dir == 1)
  {
    md.setSpeeds(-310, m2TurnSpeedPositive);
	
    while (currentTicksM1 < amount)
    {
      noInterrupts();
      currentTicksM1 = M1ticks;
	  currentM1Width = squareWidth_M1;
	  currentM2Width = squareWidth_M2;
      interrupts();
	  
	  currentTime = micros();
	  if(currentTime-prevTime == 1000)
	  {
		  //Serial.println(m2TurnSpeedPositive);
		  //Serial.println(currentM2Width);
		  //Serial.println(m1CurrentWidthNegative);
		  if (currentM2Width > currentM1Width)
		  {
			m2TurnSpeedPositive = m2TurnSpeedPositive + 1;
			OCR1B = m2TurnSpeedPositive;
		  }
		  else if (currentM2Width < currentM1Width)
		  {
			m2TurnSpeedPositive = m2TurnSpeedPositive - 1;
			OCR1B = m2TurnSpeedPositive;
		  }
		  prevTime = currentTime;
	  }
    }
  }
  else
  {
    md.setSpeeds(310, m2TurnSpeedNegative);

    while (currentTicksM1 < amount)
    {
      noInterrupts();
      currentTicksM1 = M1ticks;
	  currentM1Width = squareWidth_M1;
	  currentM2Width = squareWidth_M2;
      interrupts();
	  
	  currentTime = micros();
	  if(currentTime-prevTime == 1000)
	  {
		  //Serial.println(m2TurnSpeedNegative);
		  //Serial.println(currentM2Width);
		  //Serial.println(m1CurrentWidthNegative);
		  if (currentM2Width > currentM1Width)
		  {
			m2TurnSpeedNegative = m2TurnSpeedNegative - 1;
			OCR1B = m2TurnSpeedNegative;
		  }
		  else if (currentM2Width < currentM1Width)
		  {
			m2TurnSpeedNegative = m2TurnSpeedNegative + 1;
			OCR1B = m2TurnSpeedNegative;
		  }
		  prevTime = currentTime;
	  }
	  
    }
  }
  md.setBrakes(400, 400);

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
  
  Serial.println(m2TurnSpeedPositive);
  Serial.println(m2TurnSpeedNegative);
}
