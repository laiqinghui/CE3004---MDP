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

double fromFrontWall = 13;
double fromSideWall = 13;

//Calibration
void calibration() {
  double threshold = 0.1;
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
  }
}


//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice) {

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
    distancefromFrontWall(fromFrontWall);
    delay(wait);

    //Fine tune the calibration
    straightenTune();
    delay(wait);

    distancefromFrontWall(fromFrontWall);
    delay(wait);

    //Calibrate against right wall if there is one
    if (choice == 2)
    {
      turnPID(1, 90);
      delay(wait);

      //Fine tune the calibration
      straightenTune();
      delay(wait);

      distancefromFrontWall(fromSideWall);
      delay(wait);

      turnPID(-1, 90);
	  
	  //Fine tune the calibration
      straightenTune();
      delay(wait);
    }
  }
}

void distancefromFrontWall(double distance) {
  //Fine tune the distance from wall
  getFrontCalibrationReading(false);
  if (frontRightReading > distance)
  {
    
    while (frontRightReading > distance)
    {
      md.setSpeeds(142, 140);
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < distance)
  {
    while (frontRightReading < distance)
    {
      md.setSpeeds(-142, -140);
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
	  int delayAmount = abs(frontRightReading - frontLeftReading)*100;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
	  
      md.setSpeeds(130, 0);
	  delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading < frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*100;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
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
	  int delayAmount = abs(frontRightReading - frontLeftReading)*100;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
	  }
		
      md.setSpeeds(-130, 0);
      delay(delayAmount);
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
	while (frontRightReading > frontLeftReading)
    {
	  int delayAmount = abs(frontRightReading - frontLeftReading)*100;
	  if(delayAmount > 20)
	  {
		  delayAmount = 20;
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
	straightenTune();
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
	double rightSideReading = getRightSensorReading();
	if (rightSideReading < fromSideWall-2.5 || (rightSideReading > fromSideWall-1))
	{
		if (canSideCalibrate())
		{
			fastCalibration(1);
		}
	}
}
