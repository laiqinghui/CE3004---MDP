#include "Turning.h"
#include "Calibration Sensors.h"

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
int isSideFull[4] = {0, 0, 0, 0};
boolean acceptTony = true;

double fromFrontWall = 13;
double fromSideWall = 12.5;
double threshold = 0.1;
double checkSideWallValue = 0;

//Calibration
void calibration() {
  
  int wait = 100;

  /*
  for (int a = 0; a < 2; a++)
  {
    fastCalibration(2);
  }
  */
  
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
	
	if((double(sensorsValuesArray[0]) < 17 && double(sensorsValuesArray[0]) > 0) || (double(sensorsValuesArray[1]) < 13 && double(sensorsValuesArray[1]) > 0) || (double(sensorsValuesArray[2]) < 17 && double(sensorsValuesArray[2]) > 0))
	{
		isSideFull[0] = 1;
	}
	else
	{
		isSideFull[0] = 0;
	}
	
	fastCalibration(1);
	checkSideWallValue = getRightSensorReading();
  }
}


//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice) {
	int count = 0;
  int wait = 100;
  //Calibrate against right wall only
  if (choice == 1)
  {
    turnPID(1, 90);
    delay(wait);

    //Quick calibration against wall
    straighten();
    delay(wait);

    //Move to the distance from wall
    distancefromFrontWall(fromSideWall);

    //Fine tune the calibration
	count = 0;
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
    distancefromFrontWall(fromFrontWall);

    //Fine tune the calibration
	count = 0;
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

    distancefromFrontWall(fromFrontWall);
    delay(wait);

    //Calibrate against right wall if there is one
    if (choice == 2)
    {
      turnPID(1, 90);
	  turnAdjust(1);

      //Fine tune the calibration
	  count = 0;
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

      distancefromFrontWall(fromSideWall);
      delay(wait);

      turnPID(-1, 90);
	  turnAdjust(-1);
	  
	  //Fine tune the calibration
	  count = 0;
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
    }
  }
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
	  int delayAmount = abs(frontRightReading - frontLeftReading)*90;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  else if(delayAmount < 5)
	  {
		  delayAmount = 5;
	  }
	  
      md.setSpeeds(130, 0);
	  delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading < frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*90;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  else if(delayAmount < 5)
	  {
		  delayAmount = 5;
	  }
		
      md.setSpeeds(-130, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    while (frontRightReading < frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*90;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  else if(delayAmount < 5)
	  {
		  delayAmount = 5;
	  }
		
      md.setSpeeds(-130, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading > frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*90;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  else if(delayAmount < 5)
	  {
		  delayAmount = 5;
	  }
		
      md.setSpeeds(130, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
}

void faceNorthCalibration(){
	//Scan all sides
	int count = 0;
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
	for(int a = 1; a<4; a++)
	{
		turnPID(1, 90);
		getSensorReadingInCM();
		if((double(sensorsValuesArray[0]) < 17 && double(sensorsValuesArray[0]) > 0) || (double(sensorsValuesArray[1]) < 13 && double(sensorsValuesArray[1]) > 0) || (double(sensorsValuesArray[2]) < 17 && double(sensorsValuesArray[2]) > 0))
		{
			isSideFull[a] = 1;   
		}
		else
		{
			isSideFull[a] = 0;
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
	//isSideFull[0] == 0
	else
	{
		if(isSideFull[1] == 1)
		{
			if(isSideFull[2] == 1)
			{
				turnPID(1, 90);
			}
			//isSideFull[2] == 0
			else
			{
				if(isSideFull[3] == 0)
				{
					turnPID(1, 90);
				}
			}
		}
		//isSideFull[1] == 0 do nothing
	}
	fastCalibration(2);
	delay(150);
	turnPID(-1, 90);
	delay(150);
	turnPID(-1, 90);
}

void calibrateBeforeMoveForward() {
  acceptTony = false;
	double rightSideReading = getRightSensorReading();
	Serial.println(rightSideReading);
	if (rightSideReading < checkSideWallValue-1 || (rightSideReading > checkSideWallValue+1))
	{
		if (canSideCalibrate())
		{
			fastCalibration(1);
		}
	}
}

void tooCloseToWall(){
	getFrontCalibrationReading(false);
	int usReading = int(getUltraSoundDistance());
	
	if(usReading < 9 && usReading > 0)
	{
		while(int(getUltraSoundDistance()) < 8)
		{
			md.setSpeeds(-140, -137);
			delay(20);
			md.setBrakes(400, 400);
		}
	}
	else if(frontLeftReading < 14 && frontLeftReading > 0)
	{
		md.setSpeeds(-140, -137);
		while (frontLeftReading < 14)
		{
		  
		  getFrontCalibrationReading(true);

		}
		md.setBrakes(400, 400);
	}
	else if(frontRightReading < 14 && frontRightReading > 0)
	{
		md.setSpeeds(-140, -137);
		while (frontRightReading < 13)
		{
		  
		  getFrontCalibrationReading(true);

		}
		md.setBrakes(400, 400);
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
