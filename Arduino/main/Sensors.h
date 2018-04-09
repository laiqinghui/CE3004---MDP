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
  //PS4 y = 5819.3x - 5.9084
  //Limit is 45cm
  double frontLeftValue = sensorValues[0];
  if (frontLeftValue < 120)
  {
    sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char.
  }
  else
  {
    sensorsValuesArray[0] = (5819.3 / frontLeftValue) - 5.9084;
  }

  //------------------------------------UtraSound-----------------------------------------------------

  sensorsValuesArray[1] = (char)getUltraSound2Reading();//Center

  //------------------------------------FRONT RIGHT-----------------------------------------------------
  //PS2 y = 6325.4x - 7.9435
  //Limit is 45cm
  double frontRightValue = sensorValues[1];
  if (frontRightValue < 125)
  {
    sensorsValuesArray[2] = -1;
  }
  else
  {
    sensorsValuesArray[2] = (6325.4 / frontRightValue) - 7.9435;
  }

  //------------------------------------RIGHT -----------------------------------------------------
  //PS3 y = 5201.6x + 0.2481
  //Limit is 50cm
  double rightValue = sensorValues[3];
  if (rightValue < 105)
  {
    sensorsValuesArray[3] = -1;
  }
  else
  {
    sensorsValuesArray[3] = (5201.6 / rightValue) + 0.2481;
  }
  if(sensorsValuesArray[3] >35)
  {
	  sensorsValuesArray[3] = sensorsValuesArray[3] + 2;
  }
  
  //------------------------------------CENTER RIGHT-----------------------------------------------------
  //PS5 y = 6124.6x - 3.708
  //Limit is 50cm
  double centerRightValue = sensorValues[5];
  if (centerRightValue < 120)
  {
    sensorsValuesArray[4] = -1;
  }
  else
  {
    sensorsValuesArray[4] = (6124.6 / centerRightValue) - 3.708;
  }
    
  //------------------------------------LEFT-----------------------------------------------------
  //PS1 y = 12169x - 9.3202
  //Limit is 84cm
  double leftValue = sensorValues[2];
  if (leftValue < 140)
  {
    sensorsValuesArray[5] = -1;
  }
  else if(leftValue > 500)//491
  {
    sensorsValuesArray[5] = 20;
  }
  else
  {
    sensorsValuesArray[5] = (12169 / leftValue) - 9.3202 + 10;
  }

  //------------------------------------CENTER LEFT-----------------------------------------------------
  //PS6 y = 12464x + 0.1461
  //Limit is 80cm
  double centerLeftValue = sensorValues[4];
  if (centerLeftValue < 150)
  {
    sensorsValuesArray[6] = -1;
  }
  else if(centerLeftValue > 500 )//489
  {
    sensorsValuesArray[6] = 20;
  }
  else
  {
    sensorsValuesArray[6] = (12464 / centerLeftValue) - 8.8539 + 10;
  }

  
  
  //---------------------------Keep track of side wall------------------------------------
   if(moveForwardNumber == 1)
   {
		sideWall[2] = sideWall[1];
   }
   else if(moveForwardNumber == 2)
   {
		sideWall[2] = sideWall[0];
   }
   else if(moveForwardNumber > 2)
   {
		sideWall[2] = 0;
   }
   sideWall[1] = 0;
   sideWall[0] = 0;
   
   //Update front 2 blocks
   if (int(sensorsValuesArray[3]) < 15 && int(sensorsValuesArray[3]) > 0)
   {
		sideWall[0] = 1;
   }
   if (int(sensorsValuesArray[4]) < 15 && int(sensorsValuesArray[4]) > 0)
   {
		sideWall[1] = 1;
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

  int listOfReadingsFL[100];
  int listOfReadingsFR[150];
  int listOfReadingsL[150];
  //int listOfReadingsR[90];
  //int listOfReadingsCL[90];
  //int listOfReadingsCR[90];

//Get average reading over a number of samples
double* getIRSensorReading()
{
  sensorValues[0] = 0;
  sensorValues[1] = 0;
  sensorValues[2] = 0;
  sensorValues[3] = 0;
  sensorValues[4] = 0;
  sensorValues[5] = 0;
  
  double numberOfTimes = 1;
  int size = 100;
    
  for(int b = 0; b<numberOfTimes; b++)
  {
	     //Get Reading from Sensor
    for (int a = 0; a < size; a++)
    {
      listOfReadingsFL[a] = analogRead(frontLeft);
      listOfReadingsFR[a] = analogRead(frontRight);
      listOfReadingsL[a] = analogRead(right);
      delayMicroseconds(500);
    }
	//Get median averaged from list
    sensorValues[0] = sortAndAverage(listOfReadingsFL, size, 3);
    sensorValues[1] = sortAndAverage(listOfReadingsFR, size, 3);
    sensorValues[3] = sortAndAverage(listOfReadingsL, size, 3);
	
	    for (int a = 0; a < size; a++)
    {
      
      listOfReadingsFL[a] = analogRead(centerRight);
	  listOfReadingsFR[a] = analogRead(left);
      listOfReadingsL[a] = analogRead(centerLeft);
      delayMicroseconds(500);
    }
	for (int a = 0; a < 50; a++)
    {
	  listOfReadingsL[size+a] = analogRead(left);
      listOfReadingsL[size+a] = analogRead(centerLeft);
      delayMicroseconds(500);
    }
	//Get median averaged from list
    sensorValues[5] = sortAndAverage(listOfReadingsFL, size, 3);
    sensorValues[2] = sortAndAverage(listOfReadingsFR, 150, 3);
    sensorValues[4] = sortAndAverage(listOfReadingsL, 150, 3);
  }




	 
	  
	/*
    //Get Reading from Sensor
    for (int a = 0; a < size; a++)
    {
      listOfReadingsFL[a] = analogRead(frontLeft);
      listOfReadingsFR[a] = analogRead(frontRight);
      listOfReadingsL[a] = analogRead(left);
      listOfReadingsR[a] = analogRead(right);
      listOfReadingsCL[a] = analogRead(centerLeft);
      listOfReadingsCR[a] = analogRead(centerRight);
      delayMicroseconds(500);
    }

    //Get median averaged from list
    sensorValues[0] = sensorValues[0] + sortAndAverage(listOfReadingsFL, size, 3);
    sensorValues[1] = sensorValues[1] + sortAndAverage(listOfReadingsFR, size, 3);
    sensorValues[2] = sensorValues[2] + sortAndAverage(listOfReadingsL, size, 3);
    sensorValues[3] = sensorValues[3] + sortAndAverage(listOfReadingsR, size, 3);
    sensorValues[4] = sensorValues[4] + sortAndAverage(listOfReadingsCL, size, 3);
    sensorValues[5] = sensorValues[5] + sortAndAverage(listOfReadingsCR, size, 3);
  }
  sensorValues[0] = sensorValues[0]/numberOfTimes;
  sensorValues[1] = sensorValues[1]/numberOfTimes;
  sensorValues[2] = sensorValues[2]/numberOfTimes;
  sensorValues[3] = sensorValues[3]/numberOfTimes;
  sensorValues[4] = sensorValues[4]/numberOfTimes;
  sensorValues[5] = sensorValues[5]/numberOfTimes;
  */

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


