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

  Serial.print("M1 tunespeed: ");
  Serial.println(tuneSpeed);

 
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
    MotorPID M2pid = {100, 0, 0, 0.18};//0.163=>50 0.134=>80
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    md.setSpeeds(100, 100);


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
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
      }  


double getCir(int dir, int turnDegree)
{
  if(dir == -1)
  {
    if(turnDegree == 90)
    {
      return 16.7;
    }
    else if(turnDegree == 180)
    {
      return 16.48;
    }
    else
    {
      return 16.7;
    }
  }
  else
  {
    if(turnDegree == 90)
    {
      return 16.3;
    }
    else if(turnDegree == 180)
    {
      return 16.35;
    }
    else
    {
      return 16.7;
    }
  }
}

double getCir_test(int dir, int turnDegree)
{
  
  
  if(dir == 1)
  {
    double degree90 = 16.65;
    double degree180 = 16.4;
    double degree360 = 16.6; 
    double degree720 = 16.8;
    double degree1080 = 15;
    if(turnDegree <= 90)
    {
      return degree90;
    }
    else if(turnDegree <= 180)
    {
      double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
      double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
      
      return closenessTo90 + closenessTo180;
    }
    else if(turnDegree<= 360)
    {
      double closenessTo180 = ((turnDegree-180)/180.0)*degree360;
      double closenessTo360 = ((360 - turnDegree)/180.0)*degree180;
      
      return closenessTo180 + closenessTo360;
    }
    else if(turnDegree <= 720)
    {
      double closenessTo360 = ((turnDegree-360)/360.0)*degree720;
      double closenessTo720 = ((720 - turnDegree)/360.0)*degree360;
      
      return closenessTo360 + closenessTo720;
    }
    else if(turnDegree <= 1080)
    {
      double closenessTo720 = ((turnDegree-720)/360.0)*degree1080;
      double closenessTo1080 = ((1080 - turnDegree)/360.0)*degree720;
      
      return closenessTo1080 + closenessTo720;
    }
    
  }
  else if(dir == -1)
  {
    if(turnDegree <= 180)
    {
      return 16.7;
    }
    else if(turnDegree <= 360)
    {
      return 17.2;
    }
    else if(turnDegree <= 720)
    {
      return 17.25;
    }
    else if(turnDegree <= 1080)
    {
      return 17.2;
    }
    else
    {
      return 17.3;
    }
  }
  else
  {
    return 17.3;
  }
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
    Serial.println(getCir(dir, turnDegree));
    double cir = Pi * getCir(dir, turnDegree); //circumfrence of circle drawn when turning in cm, current diameter used is between 15.6

    int amount = abs(cir * (turnDegree/360.0) * ticksPerCM);//int to ignored decimal value //* getTurnTicksOffsetAmt(turnDegree)
    /*
     * Different Speed Values
     * md.setSpeeds(-221 * dir, 250 * dir);
     * md.setSpeeds(-168 * dir, 200 * dir);
    */
    
    int ticks = 0;
    int previousRead = 0;
    int currentValue = 0;
    
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
      md.setSpeeds(168, -199);
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
    
    while(getCalibrationReading(frontRightIR) > distance)
    {
      md.setSpeeds(118, 140);
    }
  }
  else if(getCalibrationReading(frontRightIR) < distance)
  {
    //
    while(getCalibrationReading(frontRightIR) < distance)
    {
      md.setSpeeds(-116, -140);
    }
  }
  md.setBrakes(400, 400);
}

//Calibration
void calibration()
{
  double threshold = 0.1;
  double startWall = 13.65;
  double leftWall = 13.88;
  int wait = 500;
  
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(startWall);
  delay(wait);

  //Fine tune the calibration
  while(abs(getCalibrationReading_accurate(frontRightIR) - getCalibrationReading_accurate(frontLeftIR)) > threshold)
  {
    straightenTune();
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
  while(abs(getCalibrationReading(frontRightIR) - getCalibrationReading(frontLeftIR)) > threshold)
  {
    straightenTune();
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
