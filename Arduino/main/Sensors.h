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
void quickSort(int* list, int left, int right);
int partition(int* list, int low, int high);

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
  //PS1 y = 12223x - 0.1574
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
    sensorsValuesArray[5] = (12223 / leftValue) - 0.1574;
  }

  //------------------------------------CENTER LEFT-----------------------------------------------------
  //PS6 y = 12312x + 1.4306
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
    sensorsValuesArray[6] = (12312 / centerLeftValue) + 1.4306;
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

double sortAndAverage(int* listOfReadings, int size, int amount){
	
  //Sort Reading
  quickSort(listOfReadings, 0, size -1);
 
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
double* getIRSensorReading(){
  
  int listOfReadings1[100];
  int listOfReadings2[150];
  int listOfReadings3[150];
	
  sensorValues[0] = 0;
  sensorValues[1] = 0;
  sensorValues[2] = 0;
  sensorValues[3] = 0;
  sensorValues[4] = 0;
  sensorValues[5] = 0;
  
  double numberOfTimes = 1;
  int size = 100;
  int delay = 350;
  
  int a =0;
  //Get Reading from Sensor
	for (a =0; a < size; a++)
	{
			listOfReadings1[a] = analogRead(frontLeft);
			listOfReadings2[a] = analogRead(frontRight);
			listOfReadings3[a] = analogRead(right);
			delayMicroseconds(delay);
	}
	//Get median averaged from list
	sensorValues[0] = sortAndAverage(listOfReadings1, size, 3);
	sensorValues[1] = sortAndAverage(listOfReadings2, size, 3);
	sensorValues[3] = sortAndAverage(listOfReadings3, size, 3);
	
	for (a = 0; a < size; a++)
	{
			listOfReadings1[a] = analogRead(centerRight);
			listOfReadings2[a] = analogRead(left);
			listOfReadings3[a] = analogRead(centerLeft);
			delayMicroseconds(delay);
	}
	for (a = 0; a < 50; a++)
	{
			listOfReadings2[size+a] = analogRead(left);
			listOfReadings3[size+a] = analogRead(centerLeft);
			delayMicroseconds(delay);
	}
		//Get median averaged from list
	sensorValues[5] = sortAndAverage(listOfReadings1, size, 3);
	sensorValues[2] = sortAndAverage(listOfReadings2, 150, 3);
	sensorValues[4] = sortAndAverage(listOfReadings3, 150, 3);

	return sensorValues;
}

boolean canSideCalibrate(){
	
  if (sideWall[0] == 1 && sideWall[2] == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int partition(int* list, int low, int high){

  int temp;
  int pivot;
  int pivot_pos;
  int last_small;
  int i;
  pivot_pos = (low + high)/2;
  last_small = low;
  
  
  pivot = list[pivot_pos];
  list[pivot_pos] = list[low];
  list[low] = pivot;

  for(i = low + 1; i <= high; i++){
  
    if(list[i] < pivot){
    
      temp = list[i];
      list[i] = list[++last_small];
      list[last_small] = temp; 
      
    }
    //printArr(list);
  
  }
  
  temp = list[low];
  list[low] = list[last_small];
  list[last_small] = temp;
  return last_small;


}

void quickSort(int* list, int left, int right){

  int pivot_pos;
  
  if(left > right){
    return;
  }
  
  pivot_pos = partition(list, left, right);
  //printf("%d ", pivot_pos);

  quickSort(list, left, pivot_pos - 1);
  quickSort(list, pivot_pos + 1, right);



}


