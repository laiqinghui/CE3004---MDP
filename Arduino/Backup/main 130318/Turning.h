#include "Constants.h"
#include "PID.h"
#include <EEPROM.h>
//----------------------------------------------------------------TURNING----------------------------------------------------------------
double getTurnValueOffset(int dir, int turnDegree);
void setTurnValueOffset(int dir, int turnDegree, double newValue);
double getTurnAmount(int dir, int turnDegree);
void turnPID(int dir, int degree);

double turnRight90Offset = EEPROM.read(1)/10.0;
double turnRight180Offset = EEPROM.read(0)/10.0;
double turnLeft90Offset = EEPROM.read(2)/10.0;

double getTurnValueOffset(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    if (turnDegree == 180)
    {
		return turnRight180Offset;
    }
    else
    {
		return turnRight90Offset;
    }
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
    //180
    if (turnDegree == 180)
    {
	  turnRight180Offset = newValue;
	  EEPROM.write(0, newValue*100);
      
    }
    //90
    else
    {
		turnRight90Offset = newValue;
		EEPROM.write(1, newValue*100);
    }
  }
  //Left Turn
  else
  {
     turnLeft90Offset = newValue;
	 EEPROM.write(2, newValue*100);
  }
}

double getTurnAmount(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    //180
    if (turnDegree == 180)
    {
	  return abs(47.7 * 0.5 * ticksPerCM) - turnRight180Offset;
      
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

void test()
{
	enableInterrupt( e1a, risingM1, RISING);
	md.setSpeeds(-310, 0);
	while(true)
	{
		Serial.println(squareWidth_M1);
	}
}


void turn90PIDBeta(int dir){

    double cir = 3.141 * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = ticksPerCM; //cm to counts for wheel
    int amount = cir * (90/360) * cmToCounts;//int to ignored decimal value
    unsigned long currentTicksM1 = 0;
    unsigned long currentTicksM2 = 0;
    int tuneSpeedM1 = 0;
    int tuneSpeedM2 = 0;
    int m1Speed = dir * -214;
    int m2Speed = dir * 255;

    unsigned long tuneEntryTime = 0;
    unsigned long tuneExitTime = 0;
    unsigned long interval = 0;
    
    

   if(dir == 1){//Turn right(left motor(M2) forward). Tune M2 to match M1.

      MotorPID M2 = {m2Speed , 0, 0, 0.1}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM1 < amount)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;

          Serial.print("interval");
          Serial.println(interval);
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
            tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.05)*(M2.currentErr - M2.prevErr1);
            md.setSpeeds(m1Speed, tuneSpeedM2);
            
            M2.prevTuneSpeed = tuneSpeedM2;
            M2.prevErr1 = M2.currentErr;
            Serial.print("M2.currentErr");
            Serial.println(M2.currentErr);
            tuneExitTime = micros();
          }//end of if
          
        }// end of while
        md.setBrakes(400,400);
    }//end of if
    else //turn left(right motor(M1) forward). Tune M1 to match M2. 
    {
      
      MotorPID M1 = {m1Speed , 0, 0, 0.1}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM2 < amount)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M1.currentErr = currentTicksM2 - currentTicksM1; //Positive means M1 is faster
            tuneSpeedM1 = M1.prevTuneSpeed + M1.gain*M1.currentErr + (M1.gain/0.05)*(M1.currentErr - M1.prevErr1);
            md.setSpeeds(tuneSpeedM1, m2Speed);
            
            M1.prevTuneSpeed = tuneSpeedM1;
            M1.prevErr1 = M1.currentErr;
            Serial.print("M1.currentErr");
            Serial.println(M1.currentErr);
            tuneExitTime = micros();
          }
          
        }// end of while
        md.setBrakes(400,400);
  }

      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
      
  }// end of function
