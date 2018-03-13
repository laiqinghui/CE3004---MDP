#include "UltraSound.h"

#define frontRight  A1    //Front right PS2
#define frontLeft  A3    //Front left PS4
#define left A0  // Left PS1
#define right A2 // Right PS3

//Function Declaration
double* getIRSensorReading();
char* getSensorReadingInCM();
double sortAndAverage(int* listOfReadings, int size, int amount);

/*
  sensorValues[0] Front Left Sensor
  sensorValues[1] Front Right Sensor
  sensorValues[2] Left Sensor
  sensorValues[3] Right Sensor

  sensorsValuesArray[0] Front Left Sensor
  sensorsValuesArray[1] Front Center Sensor (UltraSound)
  sensorsValuesArray[2] Front Right Sensor
  sensorsValuesArray[3] Right Sensor
  sensorsValuesArray[4] Left Sensor
*/

//These arrays need to be outside if not the values will be weird
double sensorValues[4];
char sensorsValuesArray[5];
int sideWall[3] = {0, 1, 1};

void resetSideWall() {
  sideWall[0] = 0;
  sideWall[1] = 0;
  sideWall[2] = 0;

}

char* getSensorReadingInCM() {
  /*
    Return pointer to sensors values array. Reasons for the pointer approach is to facilitate for Exploration where
    one call to this method will be sufficient for updating to RPI.
    Usage example: To get front left sensor reading just call sensorsValuesArray()[1]
  */
  getIRSensorReading();

  //PS4 y = 5546.9x - 0.7029
  //Limit is 50cm
  double frontLeftValue = sensorValues[0];
  if (frontLeftValue < 108)
  {
    sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char.
  }
  else
  {
    sensorsValuesArray[0] = (5546.9 / frontLeftValue) - 0.7029;
  }

  //Ultrasound reading
  sensorsValuesArray[1] = getUltraSoundDistance();        //getUltraSoundDistance(); //Center

  //PS2 y = 5738x - 1.1941
  //Limit is 45cm
  double frontRightValue = sensorValues[1];
  if (frontRightValue < 120)
  {
    sensorsValuesArray[2] = -1;
  }
  else
  {
    sensorsValuesArray[2] = (5738 / frontRightValue) - 1.1941;
  }

  //PS3 y = 5260x - 0.3915 //add 3 to offset
  //Limit is 50cm
  double rightValue = sensorValues[3];
  if (rightValue < 100)
  {
    sensorsValuesArray[3] = -1;
  }
  else
  {
    sensorsValuesArray[3] = (5260 / rightValue) + 1.3915;
  }

  //Keep track of side wall
  if (int(sensorsValuesArray[3]) < 14 && int(sensorsValuesArray[3]) > 0)
  {
    sideWall[2] = sideWall[1];
    sideWall[1] = sideWall[0];
    sideWall[0] = 1;
  }
  else
  {
    sideWall[2] = sideWall[1];
    sideWall[1] = sideWall[0];
    sideWall[0] = 0;
  }


  //PS1 y = 12256x - 1.948 //if value is above 70, subtract 1; //minus 2 to offset //if value is below 25, subtract 1
  //Limit is 60cm
  double leftValue = sensorValues[2];
  if (leftValue < 170)
  {
    sensorsValuesArray[4] = -1;
  }
  else
  {
    sensorsValuesArray[4] = (12256 / leftValue) - 1.948;
  }
  if (sensorsValuesArray[4] <= 25)
  {
    sensorsValuesArray[4] = sensorsValuesArray[4] - 3;
  }

  if (sensorsValuesArray[4] > 70)
  {
    sensorsValuesArray[4] = sensorsValuesArray[4] - 1;
  }


  return sensorsValuesArray;
}

double sortAndAverage(int* listOfReadings, int size, int amount)
{
  //Sort Reading
  for (int i = 0; i < size; i++)
  {
    //Find max
    int max = listOfReadings[0];
    int maxLocation = 0;
    for (int j = 0; j < size - i; j++)
    {
      if (max < listOfReadings[j])
      {
        max = listOfReadings[j];
        maxLocation = j;
      }
    }

    //Swap max with last position
    listOfReadings[maxLocation] = listOfReadings[size - 1 - i];
    listOfReadings[size - 1 - i] = max;
  }

  int lowest = (size / 2) - (amount / 2);
  int highest = (size / 2) + (amount - (amount / 2));

  double total = 0;
  for (int a = lowest; a < highest; a++)
  {
    total = total + listOfReadings[a];
  }
  return total / amount;
}

//Get average reading over a number of samples
double* getIRSensorReading()
{
  int size = 70;

  int listOfReadingsFL[size];
  int listOfReadingsFR[size];
  int listOfReadingsL[size];
  int listOfReadingsR[size];

  //Get Reading from Sensor
  for (int a = 0; a < size; a++)
  {
    listOfReadingsFL[a] = analogRead(frontLeft);
    listOfReadingsFR[a] = analogRead(frontRight);
    listOfReadingsL[a] = analogRead(left);
    listOfReadingsR[a] = analogRead(right);
    delay(1);
  }

  //Get median averaged from list
  sensorValues[0] = sortAndAverage(listOfReadingsFL, size, 3);
  sensorValues[1] = sortAndAverage(listOfReadingsFR, size, 3);
  sensorValues[2] = sortAndAverage(listOfReadingsL, size, 3);
  sensorValues[3] = sortAndAverage(listOfReadingsR, size, 3);

  return sensorValues;
}

boolean canSideCalibrate()
{
  if (sideWall[0] == 1 && sideWall[2] == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

