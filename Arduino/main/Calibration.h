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

double fromFrontWall = 12.5;
double fromSideWall = 12;
//Calibration
void calibration() {
  double threshold = 0.2;
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
    delay(wait);
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
      turnAdjust(1);
      delay(wait);

      //Fine tune the calibration
      straightenTune();
      delay(wait);

      distancefromFrontWall(fromSideWall);
      delay(wait);

      turnPID(-1, 90);
      turnAdjust(-1);
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
  //y = 5345.4x - 0.0814
  frontSensorsCalibrationCM[1] = 5345.4 * (1 / calibrationFrontSensorRaw[1]) - 0.0814;

  //Front Left
  //y = 5247x + 0.0295
  frontSensorsCalibrationCM[0] = 5247 * (1 / calibrationFrontSensorRaw[0]) + 0.0295;

  return frontSensorsCalibrationCM;
}

double* calibrationFrontSensorReading() {
  int size = 100;

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
    md.setSpeeds(108, 140);
    while (frontRightReading > distance)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < distance)
  {
    md.setSpeeds(-108, -135);
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
    md.setSpeeds(75, -120);
    while (frontRightReading > frontLeftReading)
    {
      getFrontCalibrationReading(true);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    md.setSpeeds(-69, 120);
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
      md.setSpeeds(130, 0);
      delay(30);//Change from 10 to make it faster
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
  else if (frontRightReading < frontLeftReading)
  {
    while (frontRightReading < frontLeftReading)
    {
      md.setSpeeds(-130, 0);
      delay(30);//10
      md.setBrakes(400, 400);
      getFrontCalibrationReading(false);
    }
  }
}

void turnAdjust(int dir) {
  getFrontCalibrationReading(false);
  double oldValue = getTurnValueOffset(dir);
  double difference = abs(frontRightReading - frontLeftReading) / 3;
  if(difference > 4)
  {
	  return;
  }
  //Turn Right
  if (dir == 1)
  {
    if (frontRightReading > frontLeftReading)
    {
      setTurnValueOffset(dir, oldValue + difference);
    }
    else if (frontRightReading < frontLeftReading)
    {
      setTurnValueOffset(dir, oldValue - difference);
    }
  }
  //Turn Left
  else
  {
    if (frontRightReading < frontLeftReading)
    {
      setTurnValueOffset(dir, oldValue + difference);
    }
    else if (frontRightReading > frontLeftReading)
    {
      setTurnValueOffset(dir, oldValue - difference);
    }
  }
}

void tuneM2TurnSpeed(){
  int wait = 100;

  //Check length of squarewidth for M1
  setSqWidth(0, 0);
  enableInterrupt( e1a, risingM1, RISING);

  //Check positive
  md.setSpeeds(310, 0);
  delay(wait);
  noInterrupts();
  m1CurrentWidthPositive = squareWidth_M1;
  interrupts();
  md.setBrakes(400, 400);
  setSqWidth(0, 0);
  delay(wait);

  //Check negative
  md.setSpeeds(-310, 0);
  delay(wait);
  noInterrupts();
  m1CurrentWidthNegative = squareWidth_M1;
  interrupts();
  md.setBrakes(400, 400);
  disableInterrupt(e1a);
  setSqWidth(0, 0);
  delay(wait);

  //Tune M2 to match M1 squarewidth
  setSqWidth(0, 0);
  enableInterrupt( e2b, risingM2, RISING);
  int m2CurrentWidth = 0;

  //Postive
  while (abs(m1CurrentWidthNegative - m2CurrentWidth) > 200)
  {
    md.setSpeeds(0, m2TurnSpeedPositive);
    delay(wait*2);
    noInterrupts();
    m2CurrentWidth = squareWidth_M2;
    interrupts();
    md.setBrakes(400, 400);
    setSqWidth(0, 0);
    delay(wait/2);

    if (m1CurrentWidthNegative > m2CurrentWidth)
    {
      m2TurnSpeedPositive = m2TurnSpeedPositive - 1;
    }
    else if (m1CurrentWidthNegative < m2CurrentWidth)
    {
      m2TurnSpeedPositive = m2TurnSpeedPositive + 1;
    }
  }

  //Negative
  m2CurrentWidth = 0;
  while (abs(m1CurrentWidthNegative - m2CurrentWidth) > 200)
  {
    md.setSpeeds(0, m2TurnSpeedNegative);
    delay(wait*2);
    noInterrupts();
    m2CurrentWidth = squareWidth_M2;
    interrupts();
    md.setBrakes(400, 400);
    setSqWidth(0, 0);
    delay(wait/2);

    if (m1CurrentWidthPositive > m2CurrentWidth)
    {
      m2TurnSpeedNegative = m2TurnSpeedNegative + 1;
    }
    else if (m1CurrentWidthPositive < m2CurrentWidth)
    {
      m2TurnSpeedNegative = m2TurnSpeedNegative - 1;
    }
  }
  Serial.println(m2TurnSpeedPositive);
  Serial.println(m2TurnSpeedNegative);
  disableInterrupt(e1a);
  setSqWidth(0, 0);
}
