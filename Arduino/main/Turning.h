#include "PID.h"
//----------------------------------------------------------------TURNING----------------------------------------------------------------
void setTurnValueOffset(int dir, double newValue);
void turnPID(int dir, int degree);


double offsetRight = 0.919;
double offsetLeft = 0.9165;
void turnPID(int dir, int degree){

    double cir = 3.141 * 17.6; //circumference of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = ticksPerCM; //cm to counts for wheel
	
	double amount = 0;
	
	//Right Turn
	if(dir == 1)
	{
		breakTicks = cir * 0.25 * cmToCounts * offsetRight; 
	}
	else
	{
		breakTicks = cir * 0.25 * cmToCounts * offsetLeft; 
	}		
	
    unsigned long currentTicksM1 = 0;
    unsigned long currentTicksM2 = 0;
    int tuneSpeedM1 = 0;
    int tuneSpeedM2 = 0;
    int m1Speed = dir * -250;
    int m2Speed = dir * 250;
	int error = 0;

    unsigned long tuneEntryTime = 0;
    unsigned long tuneExitTime = 0;
    unsigned long interval = 0;
    
	if(dir == 1){//Turn right(left motor(M2) forward). Tune M2 to match M1.

		m2Speed= m2Speed + 2;
		MotorPID M2 = {m2Speed , 0, 0, 0.36}; // Changed from 0.35 
		enableInterrupt( e1a, risingM1Ticks, RISING);
		enableInterrupt( e2b, risingM2Ticks, RISING);
		md.setSpeeds(m1Speed, m2Speed);
     
		while(!movementDone)//currentTicksM1 < amount
        {
			tuneEntryTime = micros();
			interval = tuneEntryTime - tuneExitTime;
          
			if(interval >= 5000)
			{ 
				noInterrupts();
				error = M1ticks - M2ticks;
				interrupts();
      
				M2.currentErr =  error; //Positive means M1 is faster
				tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.07)*(M2.currentErr - M2.prevErr1);
            
				if(!movementDone)
			        OCR1B = tuneSpeedM2;
            
				M2.prevTuneSpeed = tuneSpeedM2;
				M2.prevErr1 = M2.currentErr;
				tuneExitTime = micros();
			}//end of if
        }// end of while
    }//end of if
    else //turn left(right motor(M1) forward). Tune M1 to match M2. 
    {
		m1Speed = m1Speed;
		MotorPID M1 = {m1Speed , 0, 0, 0.79};//0.3 
		enableInterrupt( e1a, risingM1Ticks, RISING);
		enableInterrupt( e2b, risingM2Ticks, RISING);
		md.setSpeeds(m1Speed, m2Speed);
      
		while(!movementDone)
        {
			tuneEntryTime = micros();
			interval = tuneEntryTime - tuneExitTime;

			if(interval >= 5000)
			{ 
				noInterrupts();
				error = M2ticks - M1ticks;
				interrupts();
		  
				M1.currentErr =  error;
				tuneSpeedM1 = M1.prevTuneSpeed + M1.gain*M1.currentErr + (M1.gain/0.07)*(M1.currentErr - M1.prevErr1);
				
				if(!movementDone)
				  OCR1A = tuneSpeedM1;
				
				M1.prevTuneSpeed = tuneSpeedM1;
				M1.prevErr1 = M1.currentErr;
				tuneExitTime = micros();    
			}
        }// end of while
	}
    
    disableInterrupt(e1a);
    disableInterrupt(e2b);
    breakTicks = 0;
    movementDone = false;
	//Serial.print("breakTicksM2: ");
    //Serial.println(M2ticks);
    //Serial.print("breakTicksM1: ");
    //Serial.println(M1ticks);
	
    setTicks(0,0);
    setSqWidth(0,0);
}// end of function

void setTurnValueOffset(int dir, double newValue) {
  //Right Turn
  double errorChange = 0.0005;
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
}