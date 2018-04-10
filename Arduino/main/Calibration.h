  #include "Turning.h"
#include "Calibration Sensors.h"

#define frontLeftReading frontSensorsCalibrationCM[0]
#define frontRightReading frontSensorsCalibrationCM[1]

#define rightFrontReading rightSensorsCalibrationCM[0]
#define rightBackReading rightSensorsCalibrationCM[1]

//Function Declaration
void straighten();
void straightenTune();
void sideStraighten();
void distancefromWall(double distance);
void calibration();
void fastCalibration(int choice);
void turnAdjust(int dir);
void tuneM2TurnSpeed();
int isSideFull[4] = {0, 0, 0, 0};
boolean acceptTony = false;
void calibrateAgainstWall(int distance);

double fromFrontWall = 13;
double fromSideWall = 13.5;
double threshold = 0.1;
double checkSideWallValue = 0;
int wait = 70;

//Calibration
void calibration() {
  
  //Calibrate Turning
  for (int a = 0; a < 4; a++)
  {
    fastCalibration(2);
  }
  
  turnPID(1, 90);
  delay(wait);

  for (int a = 0; a < 2; a++)
  {
    //Distance from wall
    if (a == 0)
      distancefromWall(fromFrontWall);
    else
      distancefromWall(fromSideWall);
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
    
    //Distance from wall
    if (a == 0)
      distancefromWall(fromFrontWall);
    else
      distancefromWall(fromSideWall);
    delay(wait);

    //Turn to the left by 90
    turnPID(-1, 90);
  }
  
  //Check if there is a wall in front after calibration
  getSensorReadingInCM();
  if((double(sensorsValuesArray[0]) < 17 && double(sensorsValuesArray[0]) > 0) || (double(sensorsValuesArray[1]) < 13 && double(sensorsValuesArray[1]) > 0) || (double(sensorsValuesArray[2]) < 17 && double(sensorsValuesArray[2]) > 0))
  {
    isSideFull[0] = 1;
  }
  else
  {
    isSideFull[0] = 0;
  }
  
  //Set distance from right wall
  fastCalibration(1);
  checkSideWallValue = getRightSensorReading();
}


//If choice = 0 then it will only calibrate front
//If choice = 1 then it will calibrate against right wall
//If choice = 2 then it will calibrate front and right
void fastCalibration(int choice) {
  //Calibrate against right wall only
  if (choice == 1)
  {	  
    turnPID(1, 90);

    calibrateAgainstWall(fromSideWall);

    turnPID(-1, 90);
    delay(wait);
  }
  //Calibrate against front wall
  else
  {
    //Calibrate against right wall if there is one
    if (choice == 2)
    {
      turnPID(1, 90);
      turnAdjust(1);
      calibrateAgainstWall(fromSideWall);


      turnPID(-1, 90);
      turnAdjust(-1);
    }
    calibrateAgainstWall(fromFrontWall);
    }
}

void calibrateBeforeMoveForward() {
  acceptTony = false;
  //sideStraighten();
  double rightSideReading = getRightSensorReading();
  if (rightSideReading < checkSideWallValue-1 || (rightSideReading > checkSideWallValue+1.5))
  {
    if (canSideCalibrate())
    {
      fastCalibration(1);
    }
  }
}

void distancefromWall(double distance) {
  //Fine tune the distance from wall
  getFrontCalibrationReading(false);
  if (frontRightReading > distance)
  {
    md.setSpeeds(140, 140);
    while (frontRightReading > distance)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < distance)
  {
    md.setSpeeds(-140, -140);
    while (frontRightReading < distance)
    {

      getFrontCalibrationReading(true);

    }
  }
  setBrakes();
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
  setBrakes();
}

void straightenTune() {
  int min = 12;
  getFrontCalibrationReading(false);
 
  if (frontRightReading > frontLeftReading)
  {
    while (frontRightReading > frontLeftReading)
    {
    //Try proportional delay up to 20  
      md.setSpeeds(130, 0);
      delay(min);
      setBrakes();
      getFrontCalibrationReading(false);
    }
    while (frontRightReading < frontLeftReading)
    {
      md.setSpeeds(-130, 0);
      delay(min);
      setBrakes();
      getFrontCalibrationReading(false);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    while (frontRightReading < frontLeftReading)
    {
    
      md.setSpeeds(-130, 0);
      delay(min);
      setBrakes();
      getFrontCalibrationReading(false);
    }
  while (frontRightReading > frontLeftReading)
    {    
      md.setSpeeds(130, 0);
      delay(min);
      setBrakes();
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
  fastCalibration(1);
  delay(1000);
  turnPID(1, 90);
  fastCalibration(0);
  delay(1000);
  turnPID(1, 90);
}

void tooCloseToWall(){
  int usReading = int(getUltraSound2Reading());
  
  if(usReading < 7 && usReading > 0)
  {
    while(int(getUltraSound2Reading()) < 7)
    {
      md.setSpeeds(-140, -140);
      delay(20);
      setBrakes();
    }
	return;
  }
  getFrontCalibrationReading(false);
  if(frontLeftReading < 12 && frontLeftReading > 0)
  {
    md.setSpeeds(-140, -140);
    while (frontLeftReading < 12)
    {
      
      getFrontCalibrationReading(true);

    }
    setBrakes();
  }
  else if(frontRightReading < 12 && frontRightReading > 0)
  {
    md.setSpeeds(-140, -140);
    while (frontRightReading < 12)
    {
      
      getFrontCalibrationReading(true);

    }
    setBrakes();
  }
  
}

void turnAdjust(int dir) {
  getFrontCalibrationReading(false);
  //double oldValue = getTurnValueOffset(dir);
  double difference = frontRightReading - frontLeftReading;
  if(difference > 4)
  {
    return;
  }

  setTurnValueOffset(dir, difference);
}

void calibrateAgainstWall(int distance){
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distancefromWall(distance);

  //Fine tune the calibration
  int count = 0;
  straighten();
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

  distancefromWall(distance);
  delay(wait);
}

void sideStraighten()
{
  //rightFrontReading rightSensorsCalibrationCM[0]
  //rightBackReading rightSensorsCalibrationCM[1]
  getBothRightSensorReading();
  if (rightFrontReading < rightBackReading)
  {
    while (rightFrontReading < rightBackReading)
    {
      md.setSpeeds(150, -150);
      delay(15);
      setBrakes();
      getBothRightSensorReading();
    }
  }
  else if (rightFrontReading > rightBackReading)
  {
    while (rightFrontReading > rightBackReading)
    {
      md.setSpeeds(-150, 150);
      delay(15);
      setBrakes();
      getBothRightSensorReading();
    }
  }
  setBrakes();
}

