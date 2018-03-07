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
#define singlerevticks 562.215

signed long wheelDiameter = 18.849556; //Diameter is 6cm
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
void risingM1(){
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks++;
}

void risingM1Ticks(){
  M1ticks++;
  }

//ISR for Motor2(Left) encoder
void risingM2(){
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
  
  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);

  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);

 
  }

void tuneM2(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
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

   double offset = 1;
   if(distance == 9.5)
    offset = 0.92;
   
   double distanceTicks = offset  * distance * ticksPerCM;//Delibrate trimming
   unsigned long currentTicks = 0;
    
    MotorPID M1pid = {100, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {100, 0, 0, 0.117 };//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    //md.setSpeeds(100, 100);
     md.setSpeeds(269, 314);
    

    Serial.print("Target Ticks: ");
    Serial.println(distanceTicks);

   
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
      
      if(pidOn){
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 

          tuneM1(rpm, &M1pid);
          tuneM2(rpm, &M2pid);
          
          if(distance > 28.5){
            if(currentTicks < 0.7*distanceTicks){
              tuneM1(rpm, &M1pid);
              tuneM2(rpm, &M2pid);
            } else if(currentTicks < 0.85*distanceTicks){
              tuneM1(rpm*0.75, &M1pid);
              tuneM2(rpm*0.75, &M2pid);
            } else{
              tuneM1(rpm*0.5, &M1pid);
              tuneM2(rpm*0.5, &M2pid);
              }
        }  
        
          tuneExitTime = micros();
        }
      }
      noInterrupts();
      currentTicks = M1ticks;
      interrupts();
      
      if(currentTicks>=distanceTicks){
        Serial.print("BreakTicks: ");
        Serial.println(currentTicks);
        break;
      }
        
    }//end of while
      
      md.setBrakes(400,400);
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
}  

//1 is left turn and -1 is right turn
double getTurnAmount(int dir, int turnDegree){
    if(dir == 1)
    {
  		double degree90 = 51.8; //cir is 51.8
  		double degree180 = 52.6; //cir is 53.1
  		if(turnDegree < 90)
  		{
  			return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
  		}
  		else
  		{
  			double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
  			double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
  		  
  			return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
    else
    {
		  double degree90 = 48; //cir is 47.6
		  double degree180 = 49.15; //cir is 49.65
      if(turnDegree < 90)
      {
        Serial.println(abs(degree90 * (turnDegree/360.0) * ticksPerCM));
        return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
      }
      else
      {
        double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
        double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
        
        return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
}

void turn(int dir, int turnDegree){
    //1 is right, -1 is left 
    int amount = getTurnAmount(dir, turnDegree);
    int ticks = 0;
    int previousRead = 0;
    int currentValue = 0;
    
    /*
     * Different Speed Values
     * md.setSpeeds(-221 * dir, 250 * dir);
     * md.setSpeeds(-168 * dir, 200 * dir);
     * md.setSpeeds(-269 * dir, 314 * dir);
    */
    noInterrupts();
    if(dir == 1)
    {
      md.setSpeeds(-269, 314);
      while(true)
      {
        currentValue = (PINB>>5) & B001; 
        if(currentValue && !previousRead)
        {
          ticks++;
          if(ticks == amount)
          {
            md.setBrakes(400,400);
            break;        
          }
        } 
        previousRead = currentValue;
      }
    }
    else
    {
      md.setSpeeds(269, -314);
      while(true)
      {
        currentValue = (PIND>>3) & B001;
        if(currentValue && !previousRead)
        {
          ticks++;
          if(ticks == amount)
          {
            md.setBrakes(400,400);
            break;        
          }
        } 
        previousRead = currentValue;
      }
    }
    interrupts();
}
