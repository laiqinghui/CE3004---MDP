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
        md.setSpeeds(130, 0);
		    //delay(10);
		    //md.setBrakes(400, 400);
        getCalibrationReading(true);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      while(frontRightReading < frontLeftReading)
      {
        md.setSpeeds(-130, 0);
        //delay(10);
        //md.setBrakes(400, 400);
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
    while(frontRightReading > distance)
    {
      md.setSpeeds(108, 140);
      getCalibrationReading(true);
    }
  }
  else if(frontRightReading < distance)
  {  
    while(frontRightReading < distance)
    {
      md.setSpeeds(-108, -135);
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
  double leftWall = 13.;
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
  turnPID(-1, 90);
  delay(wait);
  
  //Move to the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Fine tune the calibrationu
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
  turnPID(-1, 90);
  delay(wait);
}

//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate front and left
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice)
{
  double threshold = 0.05;
  double fromWall = 13;
  int wait = 100;

  if(choice == 1)
  {
        turnPID(1, 90);
    delay(wait);
  }

  delay(wait);
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(fromWall);
  delay(wait);

  //Fine tune the calibration
  straightenTune();
  delay(wait);

  straightenTune();
  delay(wait);

  distanceFromWall(fromWall);
  delay(wait);

  if(choice == 1)
  {
    turnPID(-1, 90);
    delay(wait);
  }
  else if (choice == 2)
  {
    turnPID(1, 90);
    delay(wait);

    //Move to the distance from wall
    distanceFromWall(fromWall);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    straightenTune();
  delay(wait);

  distanceFromWall(fromWall);
  delay(wait);

    turnPID(-1, 90);
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
    delay(2);
  }
  
  //Get median averaged from list
  sensorCalibrationValues[0] = sortAndAverage(listOfReadingsFL, size);
  sensorCalibrationValues[1] = sortAndAverage(listOfReadingsFR, size);

  return sensorCalibrationValues;
}

void benForward(int rpm, double distance, boolean pidOn) {

  signed long tuneEntryTime = 0;
  signed long tuneExitTime = 0;
  signed long interval = 0;
  double offset = 1;
  if (distance == 9.5)
    offset = 0.95;

  double distanceTicks = offset  * distance * ticksPerCM;//Delibrate trimming
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  MotorPID M1pid = {260, 0, 0, 0.1};//0.1=>50
  MotorPID M2pid = {300 , 0, 0, 0.115};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);

  //md.setSpeeds(100, 100);
  md.setSpeeds(260, 300);


  Serial.print("Target Ticks: ");
  Serial.println(distanceTicks);

  while (1) {

    //Serial.print(sqWidthToRPM(squareWidth_M1));
    //Serial.print(" ");
    //Serial.println(sqWidthToRPM(squareWidth_M2));

    if (pidOn) {
      tuneEntryTime = micros();
      interval = tuneEntryTime - tuneExitTime;
      if (interval >= 2000) {

        tuneM1(rpm, &M1pid);
        tuneM2(rpm, &M2pid);

        tuneExitTime = micros();
      }
    }
    noInterrupts();
    currentTicksM1 = M1ticks;
    currentTicksM2 = M2ticks;
    interrupts();

    if (currentTicksM2 >= distanceTicks) {
      md.setBrakes(400, 400);
      break;
    }  //end of if


  }//end of while

  //md.setBrakes(400,400);
  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
}

int calibrateTurn()
{
	int ticks = 0;
	int previousRead = 0;
	int currentValue = 0;
	
	md.setSpeeds(75, -120);
    while (true)
    {
      currentValue = (PINB >> 5) & B001;
      if (currentValue && !previousRead)
      {
        ticks++;
        getCalibrationReading(true);
        if (ticks >300 && abs(frontRightReading - frontLeftReading) < 0.1)
        {
          md.setBrakes(400, 400); 
          Serial.println(frontRightReading);
          Serial.println(frontLeftReading);
          
          return ticks;
        }
        
      }
      previousRead = currentValue;
    }
}

