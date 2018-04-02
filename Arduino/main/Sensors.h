#include "UltraSound.h"

#define frontRight  A1    //Front right PS2
#define frontLeft  A3    //Front left PS4
#define left A0  // Left PS1
#define right A2 // Right PS3
#define centerRight A4 // CenterRight PS5
#define centerLeft A5 //CenterLeft PS6

//Function Declaration
double* getIRSensorReading();
char* getSensorReadingInCM();
double sortAndAverage(int* listOfReadings, int size, int amount);

//These arrays need to be outside if not the values will be weird
double sensorValues[6];
char sensorsValuesArray[7];
int sideWall[3] = {0, 1, 1};
int moveForwardNumber = 0;

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

  //------------------------------------FRONT LEFT-----------------------------------------------------
  //PS4 y = 5668.4x - 0.8187
  //Limit is 50cm
  double frontLeftValue = sensorValues[0];
  if (frontLeftValue < 108)
  {
    sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char.
  }
  else
  {
    sensorsValuesArray[0] = (5668.4 / frontLeftValue) - 0.8187;
  }

  //------------------------------------UtraSound-----------------------------------------------------
  int count = 0;
  sensorsValuesArray[1] = -1;
  while(sensorsValuesArray[1] == -1)
  {
	sensorsValuesArray[1] = (char)getUltraSound2Reading();        //getUltraSoundDistance(); //Center
	if(count > 1)//20
	{
		break;
	}
	count++;
  }

  //------------------------------------FRONT RIGHT-----------------------------------------------------
  //PS2 y = 5894.1x - 2.0988
  //Limit is 45cm
  double frontRightValue = sensorValues[1];
  if (frontRightValue < 120)
  {
    sensorsValuesArray[2] = -1;
  }
  else
  {
    sensorsValuesArray[2] = (5894.1 / frontRightValue) - 2.0988;
  }

  //------------------------------------RIGHT -----------------------------------------------------
  //PS3 y = 5413x + 1.0261
  //Limit is 50cm
  double rightValue = sensorValues[3];
  if (rightValue < 100)
  {
    sensorsValuesArray[3] = -1;
  }
  else
  {
    sensorsValuesArray[3] = (5413 / rightValue) + 1.0261;
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

  //------------------------------------LEFT-----------------------------------------------------
  //PS1 y = 12046x + 0.1764
  //Limit is 60cm
  double leftValue = sensorValues[2];
  if (leftValue < 155)
  {
    sensorsValuesArray[5] = -1;
  }
  else
  {
    sensorsValuesArray[5] = (12046 / leftValue) + 0.1764;
  }
  if (sensorsValuesArray[5] <= 23)
  {
    sensorsValuesArray[5] = sensorsValuesArray[5] - 1;
  }
  if(sensorsValuesArray[5] > 71)
	  sensorsValuesArray[5] = -1;

    //------------------------------------CENTER RIGHT-----------------------------------------------------
  //PS5 y = 6166.7x - 2.2878 //Minus 1 for offset
  //Limit is 46cm
  double centerRightValue = sensorValues[5];
  if (centerRightValue < 128)
  {
    sensorsValuesArray[4] = -1;
  }
  else
  {
    sensorsValuesArray[4] = (6166.7 / centerRightValue) - 3.2878;
  }
  
  	if(moveForwardNumber == 2)
	{
		sideWall[2] = sideWall[0];
		sideWall[1] = 0;
		sideWall[2] = 0;
		//Keep track of side wall
	  if (int(sensorsValuesArray[3]) < 14 && int(sensorsValuesArray[3]) > 0)
	  {
		sideWall[0] = 1;
	  }
	  if (int(sensorsValuesArray[5]) < 14 && int(sensorsValuesArray[5]) > 0)
	  {
		sideWall[1] = 1;
	  }
	}

  //------------------------------------CENTER LEFT-----------------------------------------------------
  //PS6  y = 11787x + 1.6425 for between 24-87cm
  //Limit is 87cm
  double centerLeftValue = sensorValues[4];
  if (centerLeftValue < 140)
  {
    sensorsValuesArray[6] = -1;
  }
  else
  {
    sensorsValuesArray[6] = (11787 / centerLeftValue) + 1.6425;
  }
  if(sensorsValuesArray[6] > 85)
	  sensorsValuesArray[6] = -1;
  if(sensorsValuesArray[6] < 27)
  {
	  sensorsValuesArray[6] = sensorsValuesArray[6] - 4;
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
  return total / (highest-lowest);
}


//Get average reading over a number of samples
double* getIRSensorReading()
{
	sensorValues[0] = 0;
	sensorValues[1] = 0;
	sensorValues[2] = 0;
	sensorValues[3] = 0;
	sensorValues[4] = 0;
	sensorValues[5] = 0;
	
	double numberOfTimes = 2;
	int size = 50;
	int listOfReadingsFL[size];
	int listOfReadingsFR[size];
	int listOfReadingsL[size*2];
	int listOfReadingsR[size];
	int listOfReadingsCL[size*2];
	int listOfReadingsCR[size];
	
	for(int b = 0; b<numberOfTimes; b++)
	{
		//Get Reading from Sensor
		for (int a = 0; a < size; a++)
		{
			listOfReadingsFL[a] = analogRead(frontLeft);
			listOfReadingsFR[a] = analogRead(frontRight);
			listOfReadingsL[a] = analogRead(left);
			listOfReadingsR[a] = analogRead(right);
			listOfReadingsCL[a] = analogRead(centerLeft);
			listOfReadingsCR[a] = analogRead(centerRight);
			delay(1);
		}
		for (int a = 0; a < size; a++)
		{
			listOfReadingsL[a+size] = analogRead(left);
			listOfReadingsCL[a+size] = analogRead(centerLeft);
			delay(1);
		}

		//Get median averaged from list
		sensorValues[0] = sensorValues[0] + sortAndAverage(listOfReadingsFL, size, 3);
		sensorValues[1] = sensorValues[1] + sortAndAverage(listOfReadingsFR, size, 3);
		sensorValues[2] = sensorValues[2] + sortAndAverage(listOfReadingsL, size*2, 3);
		sensorValues[3] = sensorValues[3] + sortAndAverage(listOfReadingsR, size, 3);
		sensorValues[4] = sensorValues[4] + sortAndAverage(listOfReadingsCL, size*2, 3);
		sensorValues[5] = sensorValues[5] + sortAndAverage(listOfReadingsCR, size, 3);
	}
	sensorValues[0] = sensorValues[0]/numberOfTimes;
	sensorValues[1] = sensorValues[1]/numberOfTimes;
	sensorValues[2] = sensorValues[2]/numberOfTimes;
	sensorValues[3] = sensorValues[3]/numberOfTimes;
	sensorValues[4] = sensorValues[4]/numberOfTimes;
	sensorValues[5] = sensorValues[5]/numberOfTimes;

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

