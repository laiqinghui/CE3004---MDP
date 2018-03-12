#include "DualVNH5019MotorShield.h"
#include "Motor.h"

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

#define rightBack A4 // Right PS5

#define frontLeftReading frontSensorsCalibrationCM[0]
#define frontRightReading frontSensorsCalibrationCM[1]

#define rightFrontReading sideSensorsCalibrationCM[0]
#define rightBackReading sideSensorsCalibrationCM[1]

//Function Declaration
void straighten();
void straightenTune();
void distanceFromWall(double distance);
void calibration();

//Front Calibration
double frontSensorsCalibrationCM[2];
double calibrationFrontSensorRaw[2];
double* getFrontCalibrationReading(boolean quick);
double* calibrationFrontSensorReading();

//Side Calibration
double sideSensorsCalibrationCM[2];
double calibrationSideSensorRaw[2];
double* getSideCalibrationReading(boolean quick);
double* calibrationSideSensorReading();



//Calibration
void calibration(){
  double threshold = 0.1;
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
  getFrontCalibrationReading(false);
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
    getFrontCalibrationReading(false);
  }
  delay(wait);

  //Fine tune distance from wall
  distanceFromWall(startWall);
  delay(wait);

  //Turn to the left by 90
  turnTest(-1, 90);
  delay(wait);
  
  //Move to the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Fine tune the calibrationu
  count = 0;
  getFrontCalibrationReading(false);
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
    getFrontCalibrationReading(false);
  }
  delay(wait);

  //Fine tune the distance from wall
  distanceFromWall(leftWall);
  delay(wait);

  //Turn to the left by 90
  turnTest(-1, 90);
  delay(wait);
}


void sideCalibration()
{
	if(canSideCalibrate())
	{
		double threshold = 0.2;
		
		getSideCalibrationReading(false);
		if(abs(rightFrontReading - rightBackReading) > threshold)
		{
			if(rightFrontReading > rightBackReading)
			{
				md.setSpeeds(-69, 120);
				while(frontRightReading > frontLeftReading)
				{
					getSideCalibrationReading(true);
				}
			}
			else if(rightFrontReading < rightBackReading)
			{
				md.setSpeeds(75, -120);
				while(frontRightReading < frontLeftReading)
				{
					getSideCalibrationReading(true);
				}
			}
		}
		md.setBrakes(400, 400);
	}
}

//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice){
  double threshold = 0.05;
  double fromWall = 14;//13
  int wait = 100;

  if(choice == 1)
  {
    turnPID(1, 90);
    delay(wait);
  }
  
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(fromWall);
  delay(wait);

  //Fine tune the calibration
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

	distanceFromWall(fromWall);
	delay(wait);

    turnPID(-1, 90);
  }  
}

void calibrateBeforeMoveForward(){
    double rightFrontSensor = 0;
    rightFrontSensor = getSideCalibrationReading(true)[0];
      Serial.println("rightFrontSensor: ");
      Serial.println(rightFrontSensor);
      if(rightFrontSensor < 10 || (rightFrontSensor > 13)){
        Serial.println("Need to calibrate");
          if(canSideCalibrate()){
            fastCalibration(1);
          } else Serial.println("No walls to calibrate");
        }
    
    }

//Side Sensor Values
double* getSideCalibrationReading(boolean quick){
  if(quick)
  {  
    calibrationSideSensorRaw[0] = analogRead(right);
    //calibrationSideSensorRaw[1] = analogRead(rightBack);
  }
  else
  {
    calibrationSideSensorReading();
  }
  
  //FrontRight
  //y = 5401x - 0.1758
  sideSensorsCalibrationCM[1] = 5401*(1/calibrationSideSensorRaw[1])-0.1758;
    
  //Front Left
  //y = 5288.6x + 0.0799
  sideSensorsCalibrationCM[0] = 5288.6*(1/calibrationSideSensorRaw[0])+ 0.0799;

  return sideSensorsCalibrationCM;
}	

double* calibrationSideSensorReading(){
  int size = 200;
  
  int listOfReadingsRF[size];
  int listOfReadingsRB[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadingsRF[a] = analogRead(right);
    listOfReadingsRB[a] = analogRead(rightBack);
    delay(1);
  }
  
  //Get median averaged from list
  calibrationSideSensorRaw[0] = sortAndAverage(listOfReadingsRF, size, 3);
  calibrationSideSensorRaw[1] = sortAndAverage(listOfReadingsRB, size, 3);

  return calibrationSideSensorRaw;
}


//Front Sensor Values
//Get average reading over a number of samples for front
double* getFrontCalibrationReading(boolean quick){
  if(quick)
  {  
    calibrationFrontSensorRaw[0] = analogRead(frontLeft);
    calibrationFrontSensorRaw[1] = analogRead(frontRight);
  }
  else
  {
    calibrationFrontSensorReading();
  }
  
  //FrontRight
  //y = 5401x - 0.1758
  frontSensorsCalibrationCM[1] = 5401*(1/calibrationFrontSensorRaw[1])-0.1758;
    
  //Front Left
  //y = 5288.6x + 0.0799
  frontSensorsCalibrationCM[0] = 5288.6*(1/calibrationFrontSensorRaw[0])+ 0.0799;

  return frontSensorsCalibrationCM;
}	

double* calibrationFrontSensorReading(){
  int size = 200;
  
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
  calibrationFrontSensorRaw[0] = sortAndAverage(listOfReadingsFL, size, 3);
  calibrationFrontSensorRaw[1] = sortAndAverage(listOfReadingsFR, size, 3);

  return calibrationFrontSensorRaw;
}

void distanceFromWall(double distance){ 
  
  //Fine tune the distance from wall
  getFrontCalibrationReading(false);
  if(frontRightReading > distance)
  { 
      
    while(frontRightReading > distance)
    {
      
      md.setSpeeds(108, 140);
      getFrontCalibrationReading(true);
    }
  }
  else if(frontRightReading < distance)
  {  
     
    while(frontRightReading < distance)
    {
      
      md.setSpeeds(-108, -135);
      getFrontCalibrationReading(true);
      
    }
  }
  md.setBrakes(400, 400);
}

void straighten(){
     
    getFrontCalibrationReading(false);
    if(frontRightReading > frontLeftReading)
    {
      md.setSpeeds(75, -120);
      while(frontRightReading > frontLeftReading)
      {
        getFrontCalibrationReading(true);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      md.setSpeeds(-69, 120);
      while(frontRightReading < frontLeftReading)
      {
        getFrontCalibrationReading(true);
      }
    }
    md.setBrakes(400, 400);
}

void straightenTune(){
    
    getFrontCalibrationReading(false);
    if(frontRightReading > frontLeftReading)
    { 
      while(frontRightReading > frontLeftReading)
      {
        md.setSpeeds(130, 0);
		    delay(50);//Change from 10 to make it faster
		    md.setBrakes(400, 400);
        getFrontCalibrationReading(false);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      while(frontRightReading < frontLeftReading)
      {
        md.setSpeeds(-130, 0);
		    delay(40);//10
		    md.setBrakes(400, 400);
        getFrontCalibrationReading(false);
      }
    }
    md.setBrakes(400, 400);
}
