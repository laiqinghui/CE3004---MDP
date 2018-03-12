#include "Turning.h"

#define frontLeftReading frontSensorsCalibrationCM[0]
#define frontRightReading frontSensorsCalibrationCM[1]

#define rightFrontReading sideSensorsCalibrationCM[0]
#define rightBackReading sideSensorsCalibrationCM[1]

//Function Declaration
void straighten();
void straightenTune();
void distanceFromWall(double distance);
void calibration();
void fastCalibration(int choice);
void turnAdjust(int dir, int amount);

//Calibration
double frontSensorsCalibrationCM[2];
double calibrationFrontSensorRaw[2];
double* getFrontCalibrationReading(boolean quick);
double* calibrationFrontSensorReading();

double fromWall = 12;
//Calibration
void calibration(){
  double threshold = 0.1;
  int wait = 200;

  for(int a = 0; a<4; a++)
  {
    fastCalibration(2);
  }
  turnPID(1, 90);
  delay(wait);
  
  for(int a = 0; a<2; a++)
  {
	  //Move to the distance from wall
	  distanceFromWall(fromWall);
	  delay(wait);

	  //Fine tune the calibration
	  int count = 0;
	  getFrontCalibrationReading(false);
	  while(abs(frontRightReading - frontLeftReading) > threshold)
	  {
		straightenTune();
		getFrontCalibrationReading(false);
		count++;
		if(count > 10)
		{
			break;
		}
	  }

	  //Fine tune distance from wall
	  distanceFromWall(fromWall);
	  delay(wait);

	  //Turn to the left by 90
	  turnPID(-1, 90);
	  delay(wait);
  }
}


//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice){
  
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
	turnAdjust(1, 90);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

	distanceFromWall(fromWall);
	delay(wait);

    turnPID(-1, 90);
	turnAdjust(-1, 90);
  }  
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
  //y = 5345.4x - 0.0814
  frontSensorsCalibrationCM[1] = 5345.4*(1/calibrationFrontSensorRaw[1])-0.0814;
    
  //Front Left
  //y = 5247x + 0.0295
  frontSensorsCalibrationCM[0] = 5247*(1/calibrationFrontSensorRaw[0])+ 0.0295;

  return frontSensorsCalibrationCM;
}	

double* calibrationFrontSensorReading(){
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
  calibrationFrontSensorRaw[0] = sortAndAverage(listOfReadingsFL, size, 3);
  calibrationFrontSensorRaw[1] = sortAndAverage(listOfReadingsFR, size, 3);

  return calibrationFrontSensorRaw;
}

void distanceFromWall(double distance){ 
  //Fine tune the distance from wall
  getFrontCalibrationReading(false);
  if(frontRightReading > distance)
  { 
	md.setSpeeds(108, 140);  
    while(frontRightReading > distance)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if(frontRightReading < distance)
  {  
	md.setSpeeds(-108, -135);
    while(frontRightReading < distance)
    {
      
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
        md.setSpeeds(180, 0);
		delay(30);//Change from 10 to make it faster
		md.setBrakes(400, 400);
        getFrontCalibrationReading(false);
      }
    }
    else if(frontRightReading < frontLeftReading)
    {
      while(frontRightReading < frontLeftReading)
      {
        md.setSpeeds(-180, 0);
		delay(30);//10
		md.setBrakes(400, 400);
        getFrontCalibrationReading(false);
      }
    }
}

void turnAdjust(int dir, int amount){
	getFrontCalibrationReading(false);
	double oldValue = getDiameterValue(dir, amount);
	//Turn Right
	if(dir == 1)
	{
		setDiameterValue(dir, amount, oldValue - (frontRightReading-frontLeftReading)/3);
	}
	//Turn Left
	else
	{
		setDiameterValue(dir, amount, oldValue - (frontLeftReading-frontRightReading)/3);
	}
}
