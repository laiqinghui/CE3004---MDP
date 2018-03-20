#include "Turning.h"

#define frontLeftReading frontSensorsCalibrationCM[0]
#define frontRightReading frontSensorsCalibrationCM[1]

#define rightFrontReading sideSensorsCalibrationCM[0]
#define rightBackReading sideSensorsCalibrationCM[1]

//Function Declaration
void straighten();
void straightenTune();
void distancefromFrontWall(double distance);
void calibration();
void fastCalibration(int choice);
void turnAdjust(int dir);
void tuneM2TurnSpeed();

//Calibration
double frontSensorsCalibrationCM[2];
double calibrationFrontSensorRaw[2];
double* getFrontCalibrationReading(boolean quick);
double* calibrationFrontSensorReading();
int isSideFull[4] = {0, 0, 0, 0};

double fromFrontWall = 12;
double fromSideWall = 13;

//Calibration
void calibration() {
  double threshold = 0.1;
  int wait = 100;

  //tuneM2TurnSpeed();
  delay(wait);

  for (int a = 0; a < 2; a++)
  {
    fastCalibration(2);
  }

  
  turnPID(1, 90);
  delay(wait);

  for (int a = 0; a < 2; a++)
  {
    if (a == 0)
    {
      //Move to the distance from wall
      distancefromFrontWall(fromFrontWall);
    }
    else
    {
      distancefromFrontWall(fromSideWall);
    }
    delay(wait);

    //Fine tune the calibration
    int count = 0;
    getFrontCalibrationReading(false);
    while (abs(frontRightReading - frontLeftReading) > threshold)
    {
      straightenTune();
      getFrontCalibrationReading(false);
      count++;
      if (count > 5)
      {
        break;
      }
    }

    if (a == 0)
    {
      //Move to the distance from wall
      distancefromFrontWall(fromFrontWall);
    }
    else
    {
      distancefromFrontWall(fromSideWall);
    }
    delay(wait);

    //Turn to the left by 90
    turnPID(-1, 90);
    getSensorReadingInCM();
		if(sensorsValuesArray[0] < 20 || sensorsValuesArray[1] < 15 || sensorsValuesArray[2] < 20)
		{
			isSideFull[a] = 1;
		}
	
  }
}


//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice) {

  int wait = 100;
  //Calibrate against right wall only
  if (choice == 0)
  {
	//Fine tune the calibration
	straightenTune();
	delay(wait);
	
	//Move to the distance from wall
    distancefromFrontWall(fromFrontWall + 0.5);
    delay(wait);
	
	//Fine tune the calibration
	straightenTune();
	delay(wait);
  }

  else if (choice == 1)
  {
    turnPID(1, 90);
    delay(wait);

    //Quick calibration against wall
    straighten();
    delay(wait);

    //Move to the distance from wall
    distancefromFrontWall(fromSideWall);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    distancefromFrontWall(fromSideWall);
    delay(wait);

    turnPID(-1, 90);
    delay(wait);
  }
  //Calibrate against front wall
  else
  {
    //Quick calibration against wall
    straighten();
    delay(wait);

    //Move to the distance from wall
    distancefromFrontWall(fromFrontWall + 0.5);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    distancefromFrontWall(fromFrontWall + 0.5);
    delay(wait);

    //Calibrate against right wall if there is one
    if (choice == 2)
    {
      turnPID(1, 90);
      turnAdjust(1);
      delay(wait);

      //Fine tune the calibration
      straightenTune();
      delay(wait);

      distancefromFrontWall(fromSideWall);
      delay(wait);

      turnPID(-1, 90);
      turnAdjust(-1);
	  
	        //Fine tune the calibration
      straightenTune();
      delay(wait);
    }
  }
}

//Front Sensor Values
//Get average reading over a number of samples for front
double* getFrontCalibrationReading(boolean quick) {
  if (quick)
  {
    calibrationFrontSensorRaw[0] = analogRead(frontLeft);
    calibrationFrontSensorRaw[1] = analogRead(frontRight);
  }
  else
  {
    calibrationFrontSensorReading();
  }

  //FrontRight
  //y = 5488.9x - 0.4385
  frontSensorsCalibrationCM[1] = 5488.9 * (1 / calibrationFrontSensorRaw[1]) - 0.4385;

  //Front Left
  //y = 5410x + 0.3186
  frontSensorsCalibrationCM[0] = 5410 * (1 / calibrationFrontSensorRaw[0]) + 0.3186;

  return frontSensorsCalibrationCM;
}

double* calibrationFrontSensorReading() {
  int size = 50;

  int listOfReadingsFL[size];
  int listOfReadingsFR[size];

  //Get Reading from Sensor
  for (int a = 0; a < size; a++)
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

void distancefromFrontWall(double distance) {
  //Fine tune the distance from wall
  getFrontCalibrationReading(false);
  if (frontRightReading > distance)
  {
    md.setSpeeds(142, 140);
    while (frontRightReading > distance)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < distance)
  {
    md.setSpeeds(-142, -140);
    while (frontRightReading < distance)
    {

      getFrontCalibrationReading(true);

    }
  }
  md.setBrakes(400, 400);
}

void straighten() {
  getFrontCalibrationReading(false);
  if (frontRightReading > frontLeftReading)
  {
    md.setSpeeds(100, -100);
    while (frontRightReading > frontLeftReading)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    md.setSpeeds(-100, 100);
    while (frontRightReading < frontLeftReading)
    {
      getFrontCalibrationReading(true);
    }
  }
  md.setBrakes(400, 400);
}

void straightenTune() {
  getFrontCalibrationReading(false);
  if (frontRightReading > frontLeftReading)
  {
    while (frontRightReading > frontLeftReading)
    {
	  //Try proportional delay up to 20
	  int delayAmount = abs(frontRightReading - frontLeftReading)*50;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  
      md.setSpeeds(100, 0);
	  delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading < frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*50;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
		
      md.setSpeeds(-100, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    while (frontRightReading < frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*50;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
		
      md.setSpeeds(-100, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading > frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*50;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
		
      md.setSpeeds(100, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
}

void turnAdjust(int dir) {
  getFrontCalibrationReading(false);
  //double oldValue = getTurnValueOffset(dir);
  double difference = abs(frontRightReading - frontLeftReading);
  if(difference > 4)
  {
	  return;
  }
  //Turn Right
  if (dir == 1)
  {
    if (frontRightReading > frontLeftReading)
    {
      setTurnValueOffset(dir, difference);
    }
    else if (frontRightReading < frontLeftReading)
    {
      setTurnValueOffset(dir, difference);
    }
  }
  //Turn Left
  else
  {
    if (frontRightReading < frontLeftReading)
    {
      setTurnValueOffset(dir, difference);
    }
    else if (frontRightReading > frontLeftReading)
    {
      setTurnValueOffset(dir, difference);
    }
  }
}

void faceNorthCalibration(){
	//Scan all sides
	for(int a = 1; a<4; a++)
	{
		turnPID(1, 90);
		getSensorReadingInCM();
		if(sensorsValuesArray[0] < 20 || sensorsValuesArray[1] < 15 || sensorsValuesArray[2] < 20)
		{
			isSideFull[a] = 1;
		}
		delay(200);
	}	
	
	if(isSideFull[0] == 1)
	{
		if(isSideFull[1] == 1)
		{
			turnPID(1, 90);
		}
	}
	else
	{
		if(isSideFull[1] == 1)
		{
			if(isSideFull[2] == 1)
			{
				turnPID(1, 90);
			}
			else
			{
				if(isSideFull[3] == 0)
				{
					turnPID(1, 90);
				}
			}
		}
	}
	fastCalibration(2);
	delay(150);
	turnPID(-1, 90);
	delay(150);
	turnPID(-1, 90);
}

void calibrateBeforeMoveForward() {
  double rightSideReading = analogRead(right);
  rightSideReading = (5260 / rightSideReading) + 1.3915;
  if (rightSideReading < fromSideWall-0.5 || (rightSideReading > fromSideWall+1))
  {
    if (canSideCalibrate())
    {
      fastCalibration(1);
    }
  }
}