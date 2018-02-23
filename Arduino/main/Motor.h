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
    MotorPID M2pid = {100, 0, 0, 0.132};//0.163=>50 0.132=>80
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

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
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setM1Ticks(0);
      setSqWidth(0,0);
      }  


double getCir(int dir, int turnDegree)
{
  if(dir == 1 && turnDegree == 1080 | turnDegree == 180)
  {
    return 16.82;
  }
  else if(dir == 1 && turnDegree == 90)
  {
    return 16.35;
  }
  else if(dir == -1 && turnDegree == 1080)
  {
    return 17.8;
  }
  else if(dir == -1 && turnDegree == 180)
  {
    return 18.2;
  }
  else if(dir == -1 && turnDegree == 90)
  {
    return 18.1;
  }
  else
  {
    return 16.82;
  }
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
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
      md.setSpeeds(-221 * dir, 250 * dir);
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
      md.setSpeeds(-221 * dir, 250 * dir);
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
      md.setSpeeds(70, -95);
      while (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR));
    }
    else if(getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR))
    {
      md.setSpeeds(-70, 95);
      while (getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR));
    }
    md.setBrakes(400, 400);
}

void distanceFromWall(double distance)
{  
  //Fine tune the distance from wall
  if(getCalibrationReading(frontRightIR) > distance)
  {
    md.setSpeeds(84, 110);
    while(getCalibrationReading(frontRightIR) > distance);
  }
  else if(getCalibrationReading(frontRightIR) < distance)
  {
    md.setSpeeds(-84, -110);
    while(getCalibrationReading(frontRightIR) < distance);
  }
  md.setBrakes(400, 400);
  Serial.println("Set");
  Serial.println(getCalibrationReading(frontRightIR));
}

//Calibration
void calibration()
{
  double threshold = 0.3;
  int wait = 1000;
  
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(12.85);
  delay(wait);
  
  //Fine tune the calibration
  for(int a = 0; a < 5; a++)
  {
    if(abs(getCalibrationReading(frontRightIR) - getCalibrationReading(frontLeftIR)) < threshold)
    {
      break;
    }
    straighten();
    delay(100);
  }
  delay(wait);

  //Fine tune distance from wall
  distanceFromWall(12.85);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(12.6);
  delay(wait);

  //Fine tune the calibration
  for(int a = 0; a < 5; a++)
  {
    if(abs(getCalibrationReading(frontRightIR) - getCalibrationReading(frontLeftIR)) < threshold)
    {
      break;
    }
    straighten();
    delay(100);
  }
  delay(wait);

  //Fine tune the distance from wall
  distanceFromWall(12.6);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
}
