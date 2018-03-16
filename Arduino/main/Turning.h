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

double offsetRight = 0.8975;
double offsetLeft = 0.8975;
void setTurnValueOffset(int dir, double newValue) {
  //Right Turn
  double errorChange = 0.0001;
  if(abs(newValue) < 0.2)
  {
	  return;
  }
  
  if(dir == 1)
  {
    if(newValue > 0)
    {
      offsetRight = offsetRight - errorChange;
    }
    else if(newValue < 0)
    {
      offsetRight = offsetRight + errorChange;
    }
  }
  else
  {
    if(newValue < 0)
    {
      offsetLeft = offsetLeft - errorChange;
    }
    else if(newValue > 0)
    {
      offsetLeft = offsetLeft + errorChange;
    }
  }
  EEPROM.write(4, ((int)(offsetRight * 10000)) >> 8);
  EEPROM.write(5, ((int)(offsetRight * 10000)) & 0xFF);
  
  EEPROM.write(6, ((int)(offsetLeft * 10000)) >> 8);
  EEPROM.write(7, ((int)(offsetLeft * 10000)) & 0xFF);
  
  
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

void turnPID(int dir, int rpm){

    double cir = 3.141 * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = ticksPerCM; //cm to counts for wheel
	
	double amount = 0;
	
	if(dir == 1)
	{
		amount = cir * 0.25 * cmToCounts * offsetRight;//int to ignored decimal value
	}
	else
	{
		amount = cir * 0.25 * cmToCounts * offsetLeft;
	
	}
	
	amount = cir * 0.25 * cmToCounts * 0.855;
	
    unsigned long currentTicksM1 = 0;
    unsigned long currentTicksM2 = 0;
    int tuneSpeedM1 = 0;
    int tuneSpeedM2 = 0;
    int m1Speed = dir * -300;
    int m2Speed = dir * 300;

    unsigned long tuneEntryTime = 0;
    unsigned long tuneExitTime = 0;
    unsigned long interval = 0;

    //Serial.print("AMOUNT");
    //Serial.println(amount);
    
   if(dir == 1){//Turn right(left motor(M2) forward). Tune M2 to match M1.

      MotorPID M2 = {m2Speed , 0, 0, 0.3}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM2 < amount || currentTicksM1 < amount)//currentTicksM1 < amount
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
            tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.07)*(M2.currentErr - M2.prevErr1);
            md.setSpeeds(m1Speed, tuneSpeedM2);
            
            M2.prevTuneSpeed = tuneSpeedM2;
            M2.prevErr1 = M2.currentErr;
            tuneExitTime = micros();
      
            //Serial.print("M2.currentErr");
            //Serial.println(M2.currentErr);
          }//end of if
          
        }// end of while
    }//end of if
    else //turn left(right motor(M1) forward). Tune M1 to match M2. 
    {
      
      MotorPID M1 = {m1Speed , 0, 0, 0.3}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM2 < amount || currentTicksM1 < amount)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M1.currentErr = currentTicksM2 - currentTicksM1; //Positive means M2 is faster
            tuneSpeedM1 = M1.prevTuneSpeed + M1.gain*M1.currentErr + (M1.gain/0.07)*(M1.currentErr - M1.prevErr1);
            md.setSpeeds(tuneSpeedM1, m2Speed);
            
            M1.prevTuneSpeed = tuneSpeedM1;
            M1.prevErr1 = M1.currentErr;
      
            tuneExitTime = micros();
            /*
            Serial.print("currentTicksM1: ");
            Serial.println(currentTicksM1);
            Serial.print("currentTicksM2: ");
            Serial.println(currentTicksM2);

            Serial.print("M1.currentErr: ");
            Serial.println(M1.currentErr);
            */
            //Serial.print("tuneSpeedM1: ");
            //Serial.println(tuneSpeedM1);
            //Serial.print("M1.currentErr: ");
            //Serial.println(M1.currentErr);
      
          }
          
        }// end of while
  }
    md.setBrakes(400, 400); 
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
	  
      
  }// end of function
