#include "Extra UltraSound.h"

#define frontRight  A1    //Front right PS2
#define frontLeft  A3    //Front left PS4
#define left A0  // Left PS1
#define right A2 // Right PS3

//Function Declaration
double* getIRSensorReading();
char getUltraSoundDistance();
char* getSensorReadingInCM();
double sortAndAverage(int* listOfReadings, int size);


char getUltraSoundDistance(){
  uint8_t DMcmd[4] = {0x22, 0x00, 0x00, 0x22}; //distance measure command
  char USValue = -1;

  for(int i=0;i<4;i++)
  {
    Serial.write(DMcmd[i]);
  }

  delay(40); //delay for 75 ms
  unsigned long timer = millis();
  while(millis() - timer < 30)
  {
    if(Serial.available()>0)
    {
      int header=Serial.read(); //0x22
      int highbyte=Serial.read();
      int lowbyte=Serial.read();
      int sum=Serial.read();//sum

      if(header == 0x22){
        if(highbyte==255)
        {
          USValue=-1;  //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte*255+lowbyte;
        }

      }
      else{
        while(Serial.available())  byte bufferClear = Serial.read();
        break;
      }
    }
  }
  return USValue;
}  


double sortAndAverage(int* listOfReadings, int size)
{
  //Sort Reading
  for (int i = 0; i < size; i++)      
  {
    //Find max
    int max = listOfReadings[0];
    int maxLocation = 0;
    for(int j = 0; j < size-i; j++)
    {
      if(max < listOfReadings[j])
      {
        max = listOfReadings[j];
        maxLocation = j;
      }
    }

    //Swap max with last position
    listOfReadings[maxLocation] = listOfReadings[size-1-i];
    listOfReadings[size-1-i] = max;
  }

  int total = 0;
  for(int a = 49; a<52; a++)
  {
    total = total + listOfReadings[a];
  }
  return total/3.0;
}

//These 2 arrays need to be outside if not the values will be weird
double sensorValues[4];
char sensorsValuesArray[5];

//Get average reading over a number of samples
double* getIRSensorReading()
{
  int size = 100;
  
  int listOfReadingsFL[size];
  int listOfReadingsFR[size];
  int listOfReadingsL[size];
  int listOfReadingsR[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadingsFL[a] = analogRead(frontLeft);
    listOfReadingsFR[a] = analogRead(frontRight);
    listOfReadingsL[a] = analogRead(left);
    listOfReadingsR[a] = analogRead(right);
    delay(1);
  }
  
  //Get median averaged from list
  sensorValues[0] = sortAndAverage(listOfReadingsFL, size);
  sensorValues[1] = sortAndAverage(listOfReadingsFR, size);
  sensorValues[2] = sortAndAverage(listOfReadingsL, size);
  sensorValues[3] = sortAndAverage(listOfReadingsR, size);

  return sensorValues;
} 

char* getSensorReadingInCM(){
    /*
    Return pointer to sensors values array. Reasons for the pointer approach is to facilitate for Exploration where
    one call to this method will be sufficient for updating to RPI.
    Usage example: To get front left sensor reading just call sensorsValuesArray()[1]
    */
  getIRSensorReading();
  
  //PS4 y = 5546.9x - 0.7029
  //Limit is 50cm
  double frontLeftValue = sensorValues[0];
  if(frontLeftValue < 108)
  {
    sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char. 
  }
  else
  {
    sensorsValuesArray[0] = (5546.9/frontLeftValue) - 0.7029;
  }
  
  //Ultrasound reading
  sensorsValuesArray[1] = getUltraSound2Reading();        //getUltraSoundDistance(); //Center


  //PS2 y = 6290.4x - 1.6964 //subtract another 2 to offset
  //Limit is 45cm
  double frontRightValue = sensorValues[1];
  if(frontRightValue < 134)
  {
    sensorsValuesArray[2] = -1;
  }
  else
  {
    sensorsValuesArray[2] = (6290.4/frontRightValue) - 2.6964;
  }

  //PS3 y = 5260x - 0.3915
  //Limit is 50cm
  double rightValue = sensorValues[3];
  if(rightValue < 100)
  {
    sensorsValuesArray[3] = -1;
  }
  else
  {
      sensorsValuesArray[3] = (5260/rightValue) - 0.3915;
  }
  
  //PS1 y = 12978x - 2.4047 //if value is above 70, subtract 1; //add 2 to offset //if value is below 25, subtract 3
  //Limit is 60cm
  double leftValue = sensorValues[2];
  if(leftValue < 170)
  {
    sensorsValuesArray[4] = -1;
  }
  else
  {
    sensorsValuesArray[4] = (12978/leftValue) - 0.4047;
  }
  if(sensorsValuesArray[4] <= 25)
  {
    sensorsValuesArray[4] = sensorsValuesArray[4] - 3;
  }
  
  if(sensorsValuesArray[4] > 70)
  {
    sensorsValuesArray[4] = sensorsValuesArray[4] - 1;
  }

  Serial.println((int)sensorsValuesArray[0]);
  Serial.println((int)sensorsValuesArray[1]);
  Serial.println((int)sensorsValuesArray[2]);
  Serial.println((int)sensorsValuesArray[3]);
  Serial.println((int)sensorsValuesArray[4]);
  
  return sensorsValuesArray;
}

