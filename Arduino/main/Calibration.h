#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Motor.h"

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//Function Declaration
void straighten();
void straightenTune();
void distanceFromWall(double distance);
double getCalibrationReading(int sensor, boolean quick);
void calibration();

void straighten()
{
    if (getCalibrationReading(frontRight, false) > getCalibrationReading(frontLeft, false))
    {
      md.setSpeeds(75, -120);
      while (getCalibrationReading(frontRight, true) > getCalibrationReading(frontLeft, true));
    }
    else if(getCalibrationReading(frontRight, false) < getCalibrationReading(frontLeft, false))
    {
      md.setSpeeds(-69, 120);
      while (getCalibrationReading(frontRight, true) < getCalibrationReading(frontLeft, true));
    }
    md.setBrakes(400, 400);
}

void straightenTune()
{
    if (getCalibrationReading(frontRight, false) > getCalibrationReading(frontLeft, false))
    {     
      while (getCalibrationReading(frontRight, true) > getCalibrationReading(frontLeft, true))
      {
        md.setSpeeds(80, 0);
      }
    }
    else if(getCalibrationReading(frontRight, false) < getCalibrationReading(frontLeft, false))
    {   
      while (getCalibrationReading(frontRight, true) < getCalibrationReading(frontLeft, true))
      {
         md.setSpeeds(-69, 0);
      }
    }
    md.setBrakes(400, 400);
}

void distanceFromWall(double distance)
{  
  //Fine tune the distance from wall
  if(getCalibrationReading(frontRight, false) > distance)
  {
    md.setSpeeds(118, 140);
    while(getCalibrationReading(frontRight, true) > distance);
  }
  else if(getCalibrationReading(frontRight, false) < distance)
  {
    md.setSpeeds(-116, -140);
    while(getCalibrationReading(frontRight, true) < distance);
  }
  md.setBrakes(400, 400);
}

//Get average reading over a number of samples
double getCalibrationReading(int sensor, boolean quick)
{
  double amount = 0;
  if(quick)
  {  
    amount = analogRead(sensor);
  }
  else
  {
    if(sensor == frontRight)
    {
      amount = getIRSensorReading()[2];
    }
    else
    {
      amount = getIRSensorReading()[0];
    }
  }
  
  if(sensor == frontRight)
  {
    //y = 5830.7(1/x) - 1.5979
    return 5830.7*(1/amount)-2.5979;
  }
  else if(sensor == frontLeft)
  {
    //y = y = 5730.2(1/x) - 1.2045
    return 5730.2*(1/amount)-1.2045;
  }
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
  while(abs(getCalibrationReading(frontRight, false) - getCalibrationReading(frontLeft, false)) > threshold)
  {
    if(count == 10)
    {
      md.setSpeeds(75, 0);
      delay(300);
      md.setBrakes(400, 400);
      count = 0;
    }
    straightenTune();
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRight, false));
    Serial.println(getCalibrationReading(frontLeft, false));
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
  count = 0;
  while(abs(getCalibrationReading(frontRight, false) - getCalibrationReading(frontLeft, false)) > threshold)
  {
    if(count == 10)
    {
      md.setSpeeds(75, 0);
      delay(300);
      md.setBrakes(400, 400);
      count = 0;
    }
    straightenTune();
    count++;
    
    Serial.println("Start");
    Serial.println(getCalibrationReading(frontRight, false));
    Serial.println(getCalibrationReading(frontLeft, false));
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
