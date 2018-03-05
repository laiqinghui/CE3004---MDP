#define frontRightIR  A1    //Front right PS2
#define frontLeftIR  A3    //Front left PS4
#define left A0  // Left PS3
#define right A2 // Right PS1

//Function Decleration
double getIRSensorReading(int sensor);
char sensorsValuesArray[5] = {0};
boolean flag=true;
uint8_t DMcmd[4] = { 
  0x22, 0x00, 0x00, 0x22}; //distance measure command
  
char getUltraSoundDistance(){
  
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

char* getSensorReadingInCM(){
    /*
    Return pointer to sensors values array. Reasons for the pointer approach is to facilitate for Exploration where
    one call to this method will be sufficient for updating to RPI.
    Usage example: To get front left sensor reading just call sensorsValuesArray()[1]
    TODO: Generate mean/median of sensors value before assigning
    */

    //PS4 y = 6511.7x - 1.958 //New equation y = 5561x - 0.5057
    //Limit is 50cm
    int frontLeftValue = getIRSensorReading(frontLeftIR);
    if(frontLeftValue < 107)
    {
      sensorsValuesArray[0] = -1;//Cannot be 0 as 0 is considered as null character in char array. I2C will terminate transmission upon the null char. 
    }
    else
    {
      sensorsValuesArray[0] = (5561/frontLeftValue) - 0.5057;
    }

		sensorsValuesArray[1] = getUltraSoundDistance(); //Center


    //PS2 y = 6414.8x - 1.2092 for values above 184 and up to 30cm
    //when x is 
    //y = 7819x - 11.552 for values 134-184 and up to 45cm
    //Limit is 45cm
    int frontRightValue = getIRSensorReading(frontRightIR);
    if(frontRightValue < 134)
    {
      sensorsValuesArray[2] = -1;
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

    //PS3 y = 5336.2x - 0.1843 for values above 200 and until 25cm //New equation y = 5926.9x - 1.7829
    //when x is 193-200 output 32.5cm
    //when x is 189-193 output 40
    //y = y = 13121x - 24.802 for values 140-189 starting from 45cm //New equation y = 14042x - 32.846

    //y = 5054.8x - 0.4362
    //Limit is 65cm
    int rightValue = getIRSensorReading(right);
    if(rightValue < 95)
    {
      sensorsValuesArray[3] = -1;
    }
    else
    {
        sensorsValuesArray[3] = (5054.8/rightValue) - 0.4362;
    }
      /*
      else if(leftValue < 200 && leftValue >= 193)
      {
        sensorsValuesArray[4] = 33.5;
      }
      else if(leftValue < 193 && leftValue > 190)
      {
        sensorsValuesArray[4] = 33.5;
      }
      else
      {
        sensorsValuesArray[4] = (14042/leftValue) - 32.846;
      }
      
      else
      {
        sensorsValuesArray[3] = -1;
      }
      
    }
  */
    //PS1 y = 6607.1x - 2.3461
    //y = 12978x - 2.4047
    //Limit is 60cm
    int leftValue = getIRSensorReading(left);

    //TEMP
    sensorsValuesArray[4] = (60.374 * pow( ( analogRead(A0)*(5.0 / 1023.0) ) , -1.16)+5);
   
    
/*   
<<<<<<< HEAD
    if(leftValue < 140)
=======
    if(leftValue < 100)
>>>>>>> 38b66b08769933e794ddd885021de5d2f58b2b1f
    {
      sensorsValuesArray[4] = -1;
    }
    else
    {
<<<<<<< HEAD
<<<<<<< HEAD
      if(leftValue >= 200)
      {
        sensorsValuesArray[4] = (6787/leftValue - 3) - 4;;//(5926.9/leftValue) - 1.7829;
      }
      /*
      else if(leftValue < 200 && leftValue >= 193)
      {
        sensorsValuesArray[4] = 33.5;
      }
      else if(leftValue < 193 && leftValue > 190)
      {
        sensorsValuesArray[4] = 33.5;
      }
      else
      {
        sensorsValuesArray[4] = (14042/leftValue) - 32.846;
      }
      */
      //else
      //{
        //sensorsValuesArray[4] = -1;
      //}
//=======
      //sensorsValuesArray[4] = (8932/leftValue) - 0.0774;
//>>>>>>> 38b66b08769933e794ddd885021de5d2f58b2b1f
    //}
=======
      sensorsValuesArray[4] = (12978/leftValue) - 2.4047;
    }
>>>>>>> 6893891a35eac4450bf25f356a91d23a9ff05b38
			
	  return sensorsValuesArray;
}

//Get average reading over a number of samples
double getCalibrationReading(int sensor, boolean quick)
{
  double amount = 0;
  if(quick)
  {  
    amount = analogRead(sensor);
  }
  else
  {
    amount = getIRSensorReading(sensor);
  }
  //Serial.println("Amount");
  //Serial.println(amount);
  
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
double getIRSensorReading(int sensor)
{
  int size = 100;
  
  int listOfReadings[size];

  //Get Reading from Sensor
  for(int a = 0; a<size; a++)
  {
    listOfReadings[a] = analogRead(sensor);
    delay(1);
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
    }\

    //Swap max with last position
    listOfReadings[maxLocation] = listOfReadings[size-1-i];
    listOfReadings[size-1-i] = max;
  }

  int total = 0;
<<<<<<< HEAD
  for(int a = 24; a<27; a++)
=======
  for(int a = 49; a<52; a++)
>>>>>>> 6893891a35eac4450bf25f356a91d23a9ff05b38
  {
    total = total + listOfReadings[a];
  }
  return total/3.0;
} 

