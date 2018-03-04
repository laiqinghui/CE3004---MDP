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
#define singlerevticks 562.215

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
  M1ticks++;
  
  
}

void risingM1Ticks(){
  M1ticks++;
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
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = 0*rpm;
    
    MotorPID M1pid = {100, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {100, 0, 0, 0.128 };//0.163=>50 0.134=>80 0.128=>90
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    md.setSpeeds(100, 100);


    while(M1ticks < distanceTicks){
      
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
      
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
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
}  


double getTurnAmount(int dir, int turnDegree)
{
    if(dir == 1)
    {
      if(turnDegree == 90)
      {
        return abs(52.4646 * (turnDegree/360.0) * ticksPerCM); //cir is 16.65 or 16.7, it is currently 16.7
      }
      else
      {
        return abs(51.7734 * (turnDegree/360.0) * ticksPerCM); //cir is 16.4 or 16.48, it is currently 16.48
      }
    }
    else
    {
      if(turnDegree == 90)
      {
        return abs(51.208 * (turnDegree/360.0) * ticksPerCM); //cir is 15.5 or 16.3, it is currently 16.3
      }
      else
      {
        return abs(51.365 * (turnDegree/360.0) * ticksPerCM); //cir is 15.7 or 16.35, it is currently 16.35
      }
    }
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left 
    int amount = getTurnAmount(dir, turnDegree);
    int ticks = 0;
    int previousRead = 0;
    int currentValue = 0;
    
    /*
     * Different Speed Values
     * md.setSpeeds(-221 * dir, 250 * dir);
     * md.setSpeeds(-168 * dir, 200 * dir);
    */
    if(dir == 1)
    {
      md.setSpeeds(-168, 196);
      while(true)
      {
        currentValue = (PINB>>5)%2;
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
      md.setSpeeds(168, -202);
      while(true)
      {
        currentValue = (PIND>>3)%2;
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
    delay(100);
}

//Methods for calibration
void straighten()
{
    if (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
    {
      md.setSpeeds(75, -120);
      while (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR));
    }
    else if(getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR))
    {
      md.setSpeeds(-69, 120);
      while (getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR));
    }
    md.setBrakes(400, 400);
}

void straightenTune()
{
    if (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
    {
      md.setSpeeds(75, 0);
      while (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR));
    }
    else if(getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR))
    {
      md.setSpeeds(-69, 0);
      while (getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR));
    }
    md.setBrakes(400, 400);
}

void distanceFromWall(double distance)
{  
  //Fine tune the distance from wall
  if(getCalibrationReading(frontRightIR) > distance)
  {
    md.setSpeeds(118, 140);
    while(getCalibrationReading(frontRightIR) > distance);
  }
  else if(getCalibrationReading(frontRightIR) < distance)
  {
    md.setSpeeds(-116, -140);
    while(getCalibrationReading(frontRightIR) < distance);
  }
  md.setBrakes(400, 400);
}

//Calibration
void calibration()
{
  double threshold = 0.1;
  double startWall = 13.65;
  double leftWall = 13.88;
  int wait = 200;
  
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(startWall);
  delay(wait);

  //Fine tune the calibration
  int count = 0;
  while(abs(getCalibrationReading_accurate(frontRightIR) - getCalibrationReading_accurate(frontLeftIR)) > threshold)
  {
    if(count == 10);
    {
      md.setSpeeds(75, 0);
      delay(500);
      md.setBrakes(400, 400);
      count == 0;
    }
    straightenTune();
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRightIR));
    Serial.println(getCalibrationReading(frontLeftIR));
    Serial.println("End");
    delay(100);
  }
  
  delay(wait);

  //Fine tune distance from wall
  distanceFromWall(startWall);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
  delay(wait);
  
  //Move to the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Fine tune the calibration
  while(abs(getCalibrationReading_accurate(frontRightIR) - getCalibrationReading_accurate(frontLeftIR)) > threshold)
  {
    if(count == 10);
    {
      md.setSpeeds(75, 0);
      delay(500);
      md.setBrakes(400, 400);
      count == 0;
    }
    straightenTune();
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRightIR));
    Serial.println(getCalibrationReading(frontLeftIR));
    Serial.println("End");
    delay(100);
  }
  delay(wait);

  //Fine tune the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
  delay(wait);
}
