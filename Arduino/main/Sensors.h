#define frontRight  A1    //Front right PS2
#define frontLeft  A3    //Front left PS4
#define left A0  // Left PS3
#define right A2 // Right PS1

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
  
  //PS4 y = 6511.7x - 1.958 //New equation y = 5561x - 0.5057
  //Limit is 50cm
  double frontLeftValue = sensorValues[0];
  if(frontLeftValue < 107)
  {
    sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char. 
  }
  else
  {
    sensorsValuesArray[0] = (5561/frontLeftValue) - 0.5057;
  }
  
  //Ultrasound reading
  sensorsValuesArray[1] = getUltraSoundDistance(); //Center


  //PS2 y = 6290.4x - 1.6964
  //Limit is 45cm
  double frontRightValue = sensorValues[1];
  if(frontRightValue < 134)
  {
    sensorsValuesArray[2] = -1;
  }
  else
  {
    sensorsValuesArray[2] = (6290.4/frontRightValue) - 1.6964;
  }

  //PS3 y = 5336.2x - 0.1843 for values above 200 and until 25cm //New equation y = 5926.9x - 1.7829
  //Limit is 65cm
  double rightValue = sensorValues[2];
  if(rightValue < 95)
  {
    sensorsValuesArray[3] = -1;
  }
  else
  {
      sensorsValuesArray[3] = (5054.8/rightValue) - 0.4362;
  }
  
  //PS1 y = 12978x - 2.4047
  //Limit is 60cm
  double leftValue = sensorValues[3];
  if(leftValue < 170)
  {
    sensorsValuesArray[4] = -1;
  }
  else
  {
    sensorsValuesArray[4] = (12978/rightValue) - 2.4047;
  }
  
  return sensorsValuesArray;
}

