#include "Motor.h"
#include "I2C.h"

//IR Sensor def
#define frontRightIR  A1    //Front right PS2
#define frontLeftIR  A3    //Front left PS4
#define left A2  // Left PS3
#define right A0 // Right PS1


//-----------------------------------------------
void setup() {
  Serial.begin(115200);
  md.init();
  initI2C();
  
 
}

void loop() {
  
  // put your main code here, to run repeatedly:
  
  turn(1, 1080);
  //md.setSpeeds(270, 300);
  while(true)
  {
  }

  
  if(dataExist()){
    Serial.println("Getting new data...");
    processInst();
    resetInBuffer();
    
    }
    
}


void straighten()
{
    if (getSensorReadingInCM(frontRightIR) > getSensorReadingInCM(frontLeftIR))
    {
      while (getSensorReadingInCM(frontRightIR) > getSensorReadingInCM(frontLeftIR))
      {
        md.setSpeeds(84, 0);
      }
  
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
    }
    else
    {
      while (getSensorReadingInCM(frontRightIR) < getSensorReadingInCM(frontLeftIR))
      {
        md.setSpeeds(-84, 0);
      }
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
    }
}

void distanceFromWall(int distance)
{
  if(getSensorReadingInCM(frontRightIR) > distance)
  {
    while(getSensorReadingInCM(frontRightIR) > distance)
      {
        md.setSpeeds(114, 140);
      }
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
  }
  else if(getSensorReadingInCM(frontRightIR) < distance)
  {
    while(getSensorReadingInCM(frontRightIR) < distance)
      {
        md.setSpeeds(-114, -140);
      }
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
  }
  
  if(getSensorReadingInCM(frontRightIR) > distance)
  {
    while(getSensorReadingInCM(frontRightIR) > distance)
      {
        md.setSpeeds(84, 110);
      }
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
  }
  else if(getSensorReadingInCM(frontRightIR) < distance)
  {
    while(getSensorReadingInCM(frontRightIR) < distance)
      {
        md.setSpeeds(-84, -110);
      }
      md.setBrakes(400, 400);
      md.setSpeeds(0, 0);
  }
}


void calibration()
{
  double threshold = 0.2;
  int wait = 1000;
  int distance = 12;
  for(int a = 0; a<2; a++)
  {
    if(abs(getSensorReadingInCM(frontRightIR) - getSensorReadingInCM(frontLeftIR)) < 0.1)
    {
      break;
    }
    straighten();
    delay(100);
  }

  Serial.println("First Calibration");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));
  delay(wait);

  //Move Backwards or Forward
  distanceFromWall(distance);
  delay(wait);

  Serial.println("Move to wall");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));
    
  while(abs(getSensorReadingInCM(frontRightIR) - getSensorReadingInCM(frontLeftIR)) > threshold)
  {
    straighten();
    delay(100);
  }
  Serial.println("Second Calibration");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));

  delay(wait);


  //Move Backwards or Forward
  distanceFromWall(distance);
  delay(wait);

  Serial.println("Second Movement");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));
  
  turn(1, 90);

  Serial.println("After Turn");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));

  //Move To Position
  distanceFromWall(distance);
  Serial.println("Last Movement");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));

  while(abs(getSensorReadingInCM(frontRightIR) - getSensorReadingInCM(frontLeftIR)) > threshold)
  {
    straighten();
    delay(100);
  }
    
  Serial.println("After turn Calibration");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));

  //Move To Position
  distanceFromWall(distance);
  Serial.println("Last Movement");
  Serial.println(getSensorReadingInCM(frontRightIR));
  Serial.println(getSensorReadingInCM(frontLeftIR));
}



void testSequence(){
  
    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 3; j++ ){
        moveForward(80, 10);
        Serial.println("\n\n\n");
        delay(500);
      }
      
    turn(-1, 90);//left
    delay(500);  
}
  
  }

void processInst(){
  
  char *buff = getinBuffer();
  char inst[5]  = {0}; 
  int index = 0;
  
  
  while(buff[index] != ';'){
    Serial.print(buff[index]);
    inst[index] = buff[index];
    Serial.print(inst[index]);
    index++;
  }
  Serial.println();
  Serial.print(inst[0]);
  switch(inst[0]){
    
    case 'm': moveForward(80, atoi(&inst[1]));
              Serial.println("m");    
              break;
    case 't': turn(-1, atoi(&inst[1]));
              break;
    default:  Serial.println(inst[0]);            
    
    }


  }

  double getSensorReadingInCM(int sensor)
{
  double reading = getIRSensorReading(sensor);
  double toReturn = (6787 / reading - 3) - 4;
  return toReturn;
  if(sensor == frontRightIR)
  {
    //y = 5830.7(1/x) - 1.5979
    return 5830.7*(1/reading)-1.5979-1;
  }
  else if(sensor == frontLeftIR)
  {
    //y = 5718.4*(1/x) - 1.9681
    return 5718.4*(1/reading)-1.9681;
  }

  
}

double getIRSensorReading(int sensor)
{
  short int size = 20;

  short int listOfReadings[size];

  //Get Reading from Sensor
  for (int a = 0; a < size; a++)
  {
    listOfReadings[a] = analogRead(sensor);
  }

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

  //Average middle 50
  short int total = 0;
  for (int a = 8; a < 12; a++)
  {
    total = total + listOfReadings[a];
  }
  return total / 4.0;
}
