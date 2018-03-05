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
    MotorPID M2pid = {100, 0, 0, 0.130 };//0.163=>50 0.134=>80 0.128=>90
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
        return abs(50.8 * (turnDegree/360.0) * ticksPerCM); //cir is 16.65 or 16.7, it is currently 16.7
      }
      else
      {
        return abs(50.8 * (turnDegree/360.0) * ticksPerCM); //cir is 16.4 or 16.48, it is currently 16.48
      }
    }
    else
    {
      if(turnDegree == 90)
      {
        return abs(48.55 * (turnDegree/360.0) * ticksPerCM); //cir is 15.5 or 16.3, it is currently 16.3
      }
      else
      {
        return abs(49.5 * (turnDegree/360.0) * ticksPerCM); //cir is 15.7 or 16.35, it is currently 16.35
      }
    }
}


void turnTemp(int dir, int turnDegree)
{
    //1 is right, -1 is left
    double offset;
    if(dir == 1)
      offset = 0.87;
    else offset = 0.88;
        
    double cir = Pi * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = singlerevticks/(6*Pi); //cm to counts for wheel
    int amount = abs(cir * (turnDegree/360.0) * cmToCounts) * offset;//0.93 FOR 50 RPM
    unsigned long M1ticksbreak = 0;
    
    enableInterrupt( e1a, risingM1Ticks, RISING);
    //md.setSpeeds(-158.921 * dir, 197.318 * dir);//50 RPM
    md.setM1Speed(-269 * dir);
    md.setM2Speed(314 * dir);
    
  
    while(1){
      noInterrupts ();
      M1ticksbreak = M1ticks;
      interrupts ();
      if(M1ticksbreak > amount){
        break;
        }
        
      
      } 

        
    disableInterrupt(e1a);
    md.setBrakes(400,400);
    Serial.println("BRAKE");
    setM1Ticks(0);
    setSqWidth(0,0);
    


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
     * md.setSpeeds(-269 * dir, 314 * dir);
    */
    if(dir == 1)
    {
      md.setSpeeds(-269, 314);
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
      md.setSpeeds(269, -314);
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
    
    setM1Ticks(0);
    setSqWidth(0,0);
}

//Methods for calibration
void straighten()
{
    if (getCalibrationReading(frontRightIR, false) > getCalibrationReading(frontLeftIR, false))
    {
      md.setSpeeds(75, -120);
      while (getCalibrationReading(frontRightIR, true) > getCalibrationReading(frontLeftIR, true));
    }
    else if(getCalibrationReading(frontRightIR, false) < getCalibrationReading(frontLeftIR, false))
    {
      md.setSpeeds(-69, 120);
      while (getCalibrationReading(frontRightIR, true) < getCalibrationReading(frontLeftIR, true));
    }
    md.setBrakes(400, 400);
}

void straightenTune()
{
    if (getCalibrationReading(frontRightIR, false) > getCalibrationReading(frontLeftIR, false))
    {     
      while (getCalibrationReading(frontRightIR, true) > getCalibrationReading(frontLeftIR, true))
      {
        md.setSpeeds(80, 0);
      }
    }
    else if(getCalibrationReading(frontRightIR, false) < getCalibrationReading(frontLeftIR, false))
    {   
      while (getCalibrationReading(frontRightIR, true) < getCalibrationReading(frontLeftIR, true))
      {
         md.setSpeeds(-69, 0);
      }
    }
    md.setBrakes(400, 400);
}

void distanceFromWall(double distance)
{  
  //Fine tune the distance from wall
  if(getCalibrationReading(frontRightIR, false) > distance)
  {
    md.setSpeeds(118, 140);
    while(getCalibrationReading(frontRightIR, true) > distance);
  }
  else if(getCalibrationReading(frontRightIR, false) < distance)
  {
    md.setSpeeds(-116, -140);
    while(getCalibrationReading(frontRightIR, true) < distance);
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
  while(abs(getCalibrationReading(frontRightIR, false) - getCalibrationReading(frontLeftIR, false)) > threshold)
  {
    if(count == 10)
    {
      md.setSpeeds(75, 0);
      delay(500);
      md.setBrakes(400, 400);
      count = 0;
    }
    straightenTune();
    Serial.println(count);
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRightIR, false));
    Serial.println(getCalibrationReading(frontLeftIR, false));
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
  /*
  //Move to the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Fine tune the calibration
  while(abs(getCalibrationReading(frontRightIR, true) - getCalibrationReading(frontLeftIR, true)) > threshold)
  {
    if(count == 10)
    {
      md.setSpeeds(75, 0);
      delay(500);
      md.setBrakes(400, 400);
      count == 0;
    }
    straightenTune();
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRightIR, false));
    Serial.println(getCalibrationReading(frontLeftIR, false));
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
  */
}
