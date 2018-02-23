#define frontRightIR  A1    //Front right PS2
#define frontLeftIR  A3    //Front left PS4
#define left A2  // Left PS3
#define right A0 // Right PS1

//Function Decleration
double getIRSensorReading(int sensor);




char sensorsValuesArray[5] = {0};



int USValue = 0;
boolean flag=true;
uint8_t DMcmd[4] = { 
  0x22, 0x00, 0x00, 0x22}; //distance measure command
  
int getUltraSoundDistance(){

	flag = true;
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
          USValue=65525;  //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte*255+lowbyte;
        }

        Serial.print("Distance=");
        Serial.println(USValue);

        flag=false;
      }
      else{
        while(Serial.available())  byte bufferClear = Serial.read();
        break;
      }
    }
  }
  return USValue;


}  

char* getSensorReadingInCM(){//Quick and dirty test i.e no avg/median of sensor value
    /*
    Return pointer to sensors values array. Reasons for the pointer approach is to facilitate for Exploration where
    one call to this method will be sufficient for updating to RPI.
    Usage example: To get front left sensor reading just call sensorsValuesArray()[1]
    TODO: Generate mean/median of sensors value before assigning
    */
		sensorsValuesArray[0] = (6787/getIRSensorReading(frontLeftIR) - 3) - 4; //Front left
		sensorsValuesArray[1] = 500; //getUltraSoundDistance() //Center
		sensorsValuesArray[2] = (6787/getIRSensorReading(frontRightIR) - 3) - 4; //Front right
		sensorsValuesArray[3] = (6787/getIRSensorReading(right) - 3) - 4; //Right
		sensorsValuesArray[4] = (6787/getIRSensorReading(left) - 3) - 4; //Left

					
	  return sensorsValuesArray;
}

double getCalibrationReading(int sensor)
{  
  double amount = getIRSensorReading(sensor);
  
  if(sensor == frontRightIR)
  {
    //y = 5830.7(1/x) - 1.5979
    return 5830.7*(1/amount)-1.5979-1;
  }
  else if(sensor == frontLeftIR)
  {
    //y = 5718.4*(1/x) - 1.9681
    return 5718.4*(1/amount)-1.9681;
  }
}	
	
//Get average reading over a number of samples
double getIRSensorReading(int sensor)
{
  int size = 20;
  
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
  for(int a = 9; a<12; a++)
  {
    total = total + listOfReadings[a];
  }
  return total/3.0;
}
	
	
	
	
int findMin(int arr[]) {
    int min1Index = (arr[0] > arr[1]) ? 1:0;
    int finalMinIndex = (arr[min1Index] > arr[2]) ? 2:min1Index;
    return finalMinIndex;
}

