#define frontRightIR  A1    //Front right PS2
#define frontLeftIR  A3    //Front left PS4
#define left A2  // Left PS3
#define right A0 // Right PS1

//Function Decleration
double getIRSensorReading(int sensor);
<<<<<<< HEAD
=======

>>>>>>> 8e0f268aac8c170e5f22169dbc2d305750d06065
char sensorsValuesArray[5] = {0};
boolean flag=true;
uint8_t DMcmd[4] = { 
  0x22, 0x00, 0x00, 0x22}; //distance measure command
  
char getUltraSoundDistance(){
  
  char USValue = 0;
  //Sending distance measure command :  0x22, 0x00, 0x00, 0x22 ;

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
          USValue=255;  //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte*255+lowbyte;
        }

        Serial.print("Distance=");
        Serial.println(USValue);

      }
      else{
        while(Serial.available())  byte bufferClear = Serial.read();
        break;
      }
    }
  }
  return USValue;


}  

char* getSensorReadingInCM(){
    /*
    Return pointer to sensors values array. Reasons for the pointer approach is to facilitate for Exploration where
    one call to this method will be sufficient for updating to RPI.
    Usage example: To get front left sensor reading just call sensorsValuesArray()[1]
    TODO: Generate mean/median of sensors value before assigning
    */
<<<<<<< HEAD
		sensorsValuesArray[0] = (6787/getIRSensorReading(frontLeftIR) - 3) - 4; //Front left
		sensorsValuesArray[1] =  getUltraSoundDistance(); //Center
		sensorsValuesArray[2] = (6787/getIRSensorReading(frontRightIR) - 3) - 4; //Front right
		sensorsValuesArray[3] = (6787/getIRSensorReading(right) - 3) - 4; //Right
		sensorsValuesArray[4] = (6787/getIRSensorReading(left) - 3) - 4; //Left
=======

    //PS4 y = 6511.7x - 1.958
    //Limit is 50cm
    int frontLeftValue = getIRSensorReading(frontLeftIR);
    if(frontLeftValue < 117)
    {
      sensorsValuesArray[0] = 0;
    }
    else
    {
      sensorsValuesArray[0] = (6511.7/frontLeftValue) - 1.958;
    }

		sensorsValuesArray[1] = 500; //getUltraSoundDistance() //Center
>>>>>>> 8e0f268aac8c170e5f22169dbc2d305750d06065

    //PS2 y = 6414.8x - 1.2092 for values above 184 and up to 30cm
    //when x is 
    //y = 7819x - 11.552 for values 134-184 and up to 45cm
    //Limit is 45cm
    int frontRightValue = getIRSensorReading(frontRightIR);
    if(frontRightValue < 134)
    {
      sensorsValuesArray[2] = 0;
    }
    else
    {
      if(frontRightValue > 184)
      {
        sensorsValuesArray[2] = (6414.8/frontRightValue) - 1.2092;
      }
      else
      {
        sensorsValuesArray[2] = (7819/frontRightValue) - 11.552;
      }
      
    }

    //PS1 y = 6607.1x - 2.3461
    //Limit is 60cm
    int rightValue = getIRSensorReading(right);
    if(rightValue < 105)
    {
      sensorsValuesArray[3] = 0;
    }
    else
    {
      sensorsValuesArray[3] = (6607.1/rightValue) - 2.3461;
    }

    //PS3 y = 5336.2x - 0.1843 for values above 200 and until 25cm
    //when x is 193-200 output 32.5cm
    //when x is 189-193 output 40
    //y = y = 13121x - 24.802 for values 140-189 starting from 45cm
    //Limit is 65cm
    int leftValue = getIRSensorReading(left);
    if(leftValue < 140)
    {
      sensorsValuesArray[4] = 0;
    }
    else
    {
      if(leftValue >= 200)
      {
        sensorsValuesArray[4] = (5336.2/leftValue) - 0.1843;
      }
      else if(leftValue < 200 && leftValue >= 193)
      {
        sensorsValuesArray[4] = 32.5;
      }
      else if(leftValue < 193 && leftValue > 189)
      {
        sensorsValuesArray[4] = 40;
      }
      else
      {
        sensorsValuesArray[4] = (13121/leftValue) - 24.802;
      }
    }
			
	  return sensorsValuesArray;
}

double getCalibrationReading(int sensor)
{  
  double amount = getIRSensorReading_calibration(sensor);
  
  if(sensor == frontRightIR)
  {
    //y = 5830.7(1/x) - 1.5979
    return 5830.7*(1/amount)-2.5979;
  }
  else if(sensor == frontLeftIR)
  {
    //y = y = 5730.2x - 1.2045
    return 5730.2*(1/amount)-1.2045;
  }
}	
	
//Get average reading over a number of samples
double getIRSensorReading_calibration(int sensor)
{
  int size = 10;
  
  int listOfReadings[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadings[a] = analogRead(sensor);
  }

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

  //Average middle 3
  short int total = 0;
  for(int a = 4; a<7; a++)
  {
    total = total + listOfReadings[a];
  }
  return total/3.0;
}

//Get average reading over a number of samples
double getIRSensorReading(int sensor)
{
  int size = 500;
  
  int listOfReadings[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadings[a] = analogRead(sensor);
  }

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

  short int total = 0;
  for(int a = 235; a<266; a++)
  {
    total = total + listOfReadings[a];
  }
  return total/30.0;
}
	
	
	
	
int findMin(int arr[]) {
    int min1Index = (arr[0] > arr[1]) ? 1:0;
    int finalMinIndex = (arr[min1Index] > arr[2]) ? 2:min1Index;
    return finalMinIndex;
}

