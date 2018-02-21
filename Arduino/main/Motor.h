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
#define Pi 3.14159265359
#define singlerevticks 1124.43

signed long wheelDiameter = 6*Pi;
signed long ticksPerCM = singlerevticks/wheelDiameter;


//------------Interrupt declarations------------
volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;
volatile unsigned long M1ticks = 0;
volatile unsigned long M2ticks = 0;


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

void m1TickCount()
{
	M1ticks++;
}

void m2TickCount()
{
  M2ticks+=2;
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
  Serial.print(currentRPM);
  Serial.print(" ");

 
  
  
  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);
  /*
  Serial.print("currentErr_M1 ");
  Serial.println(currentErr_M1);
  Serial.print("M1 tuneSpeed ");
  Serial.println(tuneSpeed);
  Serial.println();
  */
  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

 
  }

  void tuneM2(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
  //Serial.print("M2 Current RPM: ");
  Serial.println(currentRPM);

  
  
  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain*M2->currentErr + (M2->gain/0.05)*(M2->currentErr - M2->prevErr1);
  /*
  Serial.print("currentErr_M2 ");
  Serial.println(currentErr_M2);
  Serial.print("M2 tuneSpeed ");
  Serial.println(tuneSpeed);
  Serial.println();
  */
  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;
  
 
  }


void moveForward(int rpm, int distance){

   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = 0*rpm;
   
   //unsigned int rpm1 = 0;
   //unsigned int rpm2 = 0;


    
    MotorPID M1pid = {184, 0, 0, 0.1};
    MotorPID M2pid = {219, 0, 0, 0.1};
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    md.setM1Speed(184);
    delay(1);
    md.setM2Speed(219);

    
  

    while(M1ticks < distanceTicks){
      
      //rpm1 = sqWidthToRPM(squareWidth_M1);
      //rpm2 = sqWidthToRPM(squareWidth_M2);
      
      //Serial.print("M1 Current RPM: ");
      //Serial.println(rpm1);
      //Serial.print("\nM2 Current RPM: ");
      //Serial.println(rpm2);
      
      if(sqWidthToRPM(squareWidth_M2) > pidStartRPM || sqWidthToRPM(squareWidth_M1) > pidStartRPM){
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 
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
          tuneExitTime = micros();
        }
      }
    }
      md.setBrakes(400,400);
      /*
      delay(1000);
      md.setSpeeds(-230.839, -265.47);
      delay(7000);
      md.setBrakes(400,400);
      */
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
      }  


double getCir(int dir, int turnDegree)
{
  if(dir == 1 && turnDegree == 1080)
  {
    return 16.15;
  }
  else if(dir == 1 && turnDegree == 90)
  {
    return 16.05;
  }
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
    double cir = Pi * 15.6; //circumfrence of circle drawn when turning in cm, current diameter used is between 15.6
    int amount = abs(cir * (turnDegree/360.0) * ticksPerCM/2);//int to ignored decimal value //* getTurnTicksOffsetAmt(turnDegree)
    
    Serial.print("Target count: ");
    Serial.println(amount);
  
    //enableInterrupt(e1a, m1TickCount, RISING);
    //M1ticks = 0; 

    int ticks = 0;
    delay(100);

    /*
    while(ticks < amount - 100)
    {
      md.setSpeeds(-221 * dir, 250 * dir);
      md.setSpeeds(-168 * dir, 200 * dir);
    }
    */
    int previousRead = 0;
    int currentValue = 0;
    md.setSpeeds(-221 * dir, 250 * dir);
    while(true)
    {
      currentValue = (PIND>>3)%2;
      if(currentValue == 1 && previousRead == 0)
      {
        ticks++;
        if(ticks == amount)
        {
          break;        
        }
      }
      previousRead = currentValue;
    }
    md.setBrakes(400,400);
    Serial.print("Current amt: ");
    Serial.println(ticks);
    
  setSqWidth(0,0);//Reset sqWidth
  M1ticks = 0;
  
  disableInterrupt(e1a);
}

void turnCheckList(int dir, int turnDegree)
{
    //1 is right, -1 is left
    double cir = Pi * 16.15; //circumfrence of circle drawn when turning in cm, current diameter used is between 16.2
    int amount = abs(cir * (turnDegree/360.0) * ticksPerCM/2);//int to ignored decimal value //* getTurnTicksOffsetAmt(turnDegree)
    
    Serial.print("Target count: ");
    Serial.println(amount);
  
	  //enableInterrupt(e1a, m1TickCount, RISING);
    M1ticks = 0; 
    delay(100);
    
    //while(abs(M1ticks) < amount - 500)
    //{
      //md.setSpeeds(-221 * dir, 250 * dir);
    //}
    
	  while(M1ticks < amount)
    {
      md.setSpeeds(-168 * dir, 200 * dir);
    }
    
    md.setBrakes(400,400);
    Serial.print("Current amt: ");
    Serial.println(M1ticks);
    
	setSqWidth(0,0);//Reset sqWidth
	M1ticks = 0;
	
	//disableInterrupt(e1a);
}

 
void straightUsingEncoder()
{
	md.setSpeeds(-70, 85);
	enableInterrupt(e1a, m1TickCount, RISING);
	enableInterrupt(e2b, m2TickCount, RISING);
	
	int m1Speed = -70;
	
	while(true)
	{
		setM1Ticks(0);
		M2ticks = 0;
		delay(200);
		
		if(M1ticks < M2ticks)
		{
			m1Speed = m1Speed - 1;
			md.setM1Speed(m1Speed);
		}
		else if(M1ticks > M2ticks)
		{
			m1Speed = m1Speed + 1;
			md.setM1Speed(m1Speed);
		}
		Serial.println("Next Update");
		Serial.println(m1Speed);
		Serial.println("70");
    Serial.println(M1ticks);
    Serial.println(M2ticks);
		Serial.println(M1ticks - M2ticks);
	}
	
} 

