#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"
#include "Calibration.h"

//----------------------------------------------------------------FORWARD----------------------------------------------------------------  
void moveForwardOneGrid(int rpm);
void moveForwardBeta(int rpm, double distance);
void calibrateBeforeMoveForward();

void moveForwardOneGrid(int rpm){
	
	unsigned long tuneEntryTime = 0;
	unsigned long tuneExitTime = 0;
	unsigned long interval = 0;
	double distanceTicks = 0.96 * 9.5 * ticksPerCM;
	unsigned long currentTicksM1 = 0;
	unsigned long currentTicksM2 = 0;
	  
	int m1setSpeed = 100;
	int m2setSpeed = 100;

	MotorPID M1pid = {m1setSpeed, 0, 0, 0.110};//LOUNGE: 0.110=>50/ LAB: 0.115=>50 || LAB: 0.110=>90/ LAB: 0.115=>50
	MotorPID M2pid = {m2setSpeed , 0, 0, 0.138};//LOUNGE: 0.150->50/LAB: 0.150->50 || LAB: 0.135=>90/ LAB: 
	
	//Check using right side sensor if need to calibrate
	calibrateBeforeMoveForward();
	
	enableInterrupt( e1a, risingM1, RISING);
	enableInterrupt( e2b, risingM2, RISING);
	setTicks(0,0);

	md.setSpeeds(m1setSpeed,m2setSpeed);
	while(1)
	{
		//Serial.print(sqWidthToRPM(squareWidth_M1));
		//Serial.print(" ");
		//Serial.println(sqWidthToRPM(squareWidth_M2));
	  
		noInterrupts();
		currentTicksM1 = M1ticks;
		currentTicksM2 = M2ticks;
		interrupts();
		      
		if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks)
		{
			md.setBrakes(400, 400);
			break;
		}
      
		tuneEntryTime = micros();//Can try removing interval for single grid movement
		interval = tuneEntryTime - tuneExitTime;
		if(interval >= 5000)
		{
			tuneMotors(rpm, &M1pid, &M2pid);
            tuneExitTime = micros();
        }

    }//end of while
      
    disableInterrupt(e1a);
    disableInterrupt(e2b);
    setTicks(0,0);
    setSqWidth(0,0);
}

void moveForwardBeta(int rpm, double distance){
	
	double rightFrontSensor = 0;
	unsigned long tuneEntryTime = 0;
	unsigned long tuneExitTime = 0;
	unsigned long interval = 0;
	double distanceTicks = 0.98 * distance * ticksPerCM;
	unsigned long currentTicksM1 = 0;
	unsigned long currentTicksM2 = 0;
   
	int m1setSpeed = 100;
	int m2setSpeed = 100;
	
	//Check using right side sensor if need to calibrate
	calibrateBeforeMoveForward();
	
	MotorPID M1pid = {m1setSpeed, 0, 0, 0.110};//0.1=>50
	MotorPID M2pid = {m2setSpeed , 0, 0, 0.125};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

	enableInterrupt( e1a, risingM1, RISING);
	enableInterrupt( e2b, risingM2, RISING);
	setTicks(0,0);

	md.setSpeeds(m1setSpeed,m2setSpeed);

	while(1)
	{
	    noInterrupts();
		currentTicksM1 = M1ticks;
		currentTicksM2 = M2ticks;
		interrupts();
      
		if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks)
		{
			md.setBrakes(400, 400);
			break;
		}
      
		tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000)
		{
			tuneMotors(rpm, &M1pid, &M2pid);
			
			//Gradual breaking
			/*
			if(currentTicksM1 < 0.7*distanceTicks){
			  //tuneM1(rpm, &M1pid);
			  //tuneM2(rpm, &M2pid);
			  tuneMotors(rpm, &M1pid, &M2pid);
			}else if(currentTicksM1 < 0.85*distanceTicks){
			  //tuneM1(rpm*0.75, &M1pid);
			  //tuneM2(rpm*0.75, &M2pid);
			  tuneMotors(rpm*0.75, &M1pid, &M2pid);
			}else{
			  //tuneM1(rpm*0.4, &M1pid);
			  //tuneM2(rpm*0.4, &M2pid);
			  tuneMotors(rpm*0.4, &M1pid, &M2pid);
			}
			*/
			tuneExitTime = micros();
        }

    }//end of while
      
    disableInterrupt(e1a);
    disableInterrupt(e2b);
    setTicks(0,0);
    setSqWidth(0,0);
}

void calibrateBeforeMoveForward(){
	double rightSideReading = analogRead(right);
	rightSideReading = (5260/rightSideReading) + 1.3915;
    if(rightSideReading < 10 || (rightSideReading > 12))
	{
        if(canSideCalibrate())
		{
			fastCalibration(1);
        }     
    }
}
