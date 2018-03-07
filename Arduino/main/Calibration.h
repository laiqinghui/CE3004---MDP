#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Motor.h"

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

#define frontLeftReading sensorsCalibrationCM[0]
#define frontRightReading sensorsCalibrationCM[1]

//Function Declaration
void straighten();
void straightenTune();
void distanceFromWall(double distance);
double* getCalibrationReading(boolean quick);
void calibration();
double* calibrationSensorReading();

double sensorCalibrationValues[2];
double sensorsCalibrationCM[2];

void straighten()
{
    getCalibrationReading(false);
    if(frontRightReading > frontLeftReading)
    {
      md.setSpeeds(75, -120);
      while(frontRightReading > frontLeftReading)
      {
        getCalibrationReading(true);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      md.setSpeeds(-69, 120);
      while(frontRightReading < frontLeftReading)
      {
        getCalibrationReading(true);
      }
    }
    md.setBrakes(400, 400);
}

void straightenTune()
{
    getCalibrationReading(false);
    if(frontRightReading > frontLeftReading)
    { 
      while(frontRightReading > frontLeftReading)
      {
        md.setSpeeds(110, 0);
        getCalibrationReading(true);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      while(frontRightReading < frontLeftReading)
      {
        md.setSpeeds(-110, 0);
        getCalibrationReading(true);
      }
    }
    md.setBrakes(400, 400);
}

void distanceFromWall(double distance)
{  
  //Fine tune the distance from wall
  getCalibrationReading(false);
  if(frontRightReading > distance)
  {
    md.setSpeeds(118, 140);
    while(frontRightReading > distance)
    {
      getCalibrationReading(true);
    }
  }
  else if(frontRightReading < distance)
  {
    md.setSpeeds(-116, -140);
    while(frontRightReading > distance)
    {
      getCalibrationReading(true);
    }
  }
  md.setBrakes(400, 400);
}

//Get average reading over a number of samples
double* getCalibrationReading(boolean quick)
{
  if(quick)
  {  
    sensorCalibrationValues[0] = analogRead(frontLeft);
    sensorCalibrationValues[1] = analogRead(frontRight);
  }
  else
  {
    calibrationSensorReading();
  }
  
  //FrontRight
  //y = 5401x - 0.1758
  sensorsCalibrationCM[1] = 5401*(1/sensorCalibrationValues[1])-0.1758;
    
  //Front Left
  //y = 5288.6x + 0.0799
  sensorsCalibrationCM[0] = 5288.6*(1/sensorCalibrationValues[0])+ 0.0799;

  return sensorsCalibrationCM;
}	

//Calibration
void calibration()
{
  double threshold = 0.2;
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
  getCalibrationReading(false);
  while(abs(frontRightReading - frontLeftReading) > threshold)
  {
    Serial.println("Set");
    Serial.println(frontRightReading);
    Serial.println(frontLeftReading);
    if(count == 4)
    {
      md.setSpeeds(75, 0);
      delay(300);
      md.setBrakes(400, 400);
      count = 0;
    }
    straightenTune();
    count++;
    
    delay(100);
    getCalibrationReading(false);
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
  getCalibrationReading(false);
  while(abs(frontRightReading - frontLeftReading) > threshold)
  {
    if(count == 4)
    {
      md.setSpeeds(75, 0);
      delay(300);
      md.setBrakes(400, 400);
      count = 0;
    }
    straightenTune();
    count++;
    
    delay(100);
    getCalibrationReading(false);
  }
  delay(wait);

  //Fine tune the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
  delay(wait);
}

//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate front and left
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice)
{
  double threshold = 0.1;
  double startWall = 13.65;
  double leftWall = 13.88;
  int wait = 200;
  
  //Quick calibration against wall
  delay(100);
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(startWall);
  delay(wait);

  //Fine tune the calibration
  straightenTune();
  delay(wait);

  straightenTune();
  delay(wait);

  if(choice == 1)
  {
    turn(-1, 90);
    delay(wait);

    //Move to the distance from wall
    distanceFromWall(startWall);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    straightenTune();
  delay(wait);

    turn(1, 90);
  }
  else if (choice == 2)
  {
    turn(1, 90);
    delay(wait);

    //Move to the distance from wall
    distanceFromWall(startWall);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    straightenTune();
  delay(wait);

    turn(-1, 90);
  }  
}


double* calibrationSensorReading()
{
  int size = 100;
  
  int listOfReadingsFL[size];
  int listOfReadingsFR[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadingsFL[a] = analogRead(frontLeft);
    listOfReadingsFR[a] = analogRead(frontRight);
    delay(1);
  }
  
  //Get median averaged from list
  sensorCalibrationValues[0] = sortAndAverage(listOfReadingsFL, size);
  sensorCalibrationValues[1] = sortAndAverage(listOfReadingsFR, size);

  return sensorCalibrationValues;
}

