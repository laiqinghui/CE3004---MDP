//For increase interrupt speed
//#define NEEDFORSPEED
//#define INTERRUPT_FLAG_PIN3 M1Ticks
#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"

DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//------------PID Structs------------
struct MotorPID {
  float prevTuneSpeed;
  float currentErr;
  float prevErr1;
  float gain;
};

//------------Other constants and declrations----------
#define Pi 3.1416
#define singlerevticks 1124.43

signed long wheelDiameter = 6*Pi;
signed long ticksPerCM = singlerevticks/wheelDiameter;


//------------Interrupt declarations------------
volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;
volatile unsigned long M1ticks = 0;


volatile int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;
volatile signed long entry_time_M2 = 0; 


//ISR for Motor1(Right) encoder 
void risingM1()
{
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks+=2;
  
  
}

//ISR for Motor2(Left) encoder
void risingM2()
{
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
  
  
}

void setM1Ticks(int ticks){
	M1ticks = ticks;
}

void setSqWidth(int M1, int M2){
	squareWidth_M1 = M1;
	squareWidth_M2 = M2;
}

signed long sqWidthToRPM(int sqWidth){

  if(sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
  signed long sqwavesOneS = 1000000/sqWidth;//1/(sqWidth/1000000)
  signed long sqwavesOneM = sqwavesOneS*60;
  signed long revPerMin = sqwavesOneM/sqwavesPerRev;

  return revPerMin;
 
  }


void tuneM1(int desiredRPM, MotorPID *M1){


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);
  
  //Serial.print("M1 Current RPM: ");
  //Serial.print(currentRPM);
  //Serial.print(" ");

  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);

  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

 
  }

  void tuneM2(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
  //Serial.print("M2 Current RPM: ");
  //Serial.println(currentRPM);

  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain*M2->currentErr + (M2->gain/0.05)*(M2->currentErr - M2->prevErr1);

  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;
  
 
  }


void moveForward(int rpm, int distance, boolean pidOn){

   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = 0*rpm;
   
   //unsigned int rpm1 = 0;
   //unsigned int rpm2 = 0;


    
    MotorPID M1pid = {100, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {100, 0, 0, 0.132};//0.163=>50 0.132=>80
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    //md.setM1Speed(158);//184
    //delay(3);
    //md.setM2Speed(197);//219
    //md.setSpeeds(155, 178);//
    md.setSpeeds(100, 100);//

    
  

    while(M1ticks < distanceTicks){
      
      Serial.print(sqWidthToRPM(squareWidth_M1));
      Serial.print(" ");
      Serial.println(sqWidthToRPM(squareWidth_M2));
      
      if(pidOn){
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 

          tuneM1(rpm, &M1pid);
          tuneM2(rpm, &M2pid);
          /*
          if(M1ticks < 0.7*distanceTicks){
            tuneM1(rpm, &M1pid);
            tuneM2(rpm, &M2pid);
          } else if(M1ticks < 0.85*distanceTicks){
            tuneM1(rpm*0.75, &M1pid);
            tuneM2(rpm*0.75, &M2pid);
          } else{
            tuneM1(rpm*0.5, &M1pid);
            tuneM2(rpm*0.5, &M2pid);
            }
            */
          tuneExitTime = micros();
        }
      }
    }//end of while
      
      md.setBrakes(400,400);
      md.setM1Brake(400);
      //delay(15);
      md.setM2Brake(400);
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
      }  
/*
void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
    double cir = Pi * 17.5; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    int amount = abs(cir * (turnDegree/360.0) * ticksPerCM);//int to ignored decimal value //* getTurnTicksOffsetAmt(turnDegree)
    
    Serial.print("Target count: ");
    Serial.println(amount);
  
	  enableInterruptFast(e1a, CHANGE);
    M1Ticks = 0; 
    md.setSpeeds(-159 * dir, 197 * dir);//80 RPM
    
    while(abs(M1Ticks) < amount - 500)
    {
    }
    
	  md.setSpeeds(-270 * dir, 300 * dir)
	  while(abs(M1Ticks) < amount - 500)
    {
    }
    

    md.setBrakes(400,400);
    Serial.print("Current amt: ");
    Serial.println(M1Ticks);
    
	setSqWidth(0,0);//Reset sqWidth
	M1Ticks = 0;
	
	disableInterrupt(e1a);
}
*/
/*  
void straightUsingEncoder()
{
	md.setSpeeds(270, 300);
	PCintPort::attachInterrupt(e1a, &risingM1, RISING);
	PCintPort::attachInterrupt(e2b, &risingM2, RISING);
	
	int m1Speed = 270;
	
	while(true)
	{
		setM1Ticks(0);
		setM2Ticks(0);
		delay(100);
		
		if(M1Ticks < M2Ticks)
		{
			m1Speed = m1Speed + 1;
			md.setM1Speed(m1Speed);
		}
		else if(M1Ticks > M2Ticks)
		{
			m1Speed = m1Speed -1;
			md.setM1Speed(m1Speed);
		}
		Serial.println("Next Update");
		Serial.println(m1Speed);
		Serial.println("300");
		Serial.println(M1Ticks - M2Ticks);
	}
	
} 
*/
