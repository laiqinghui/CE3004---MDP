#include "Motor.h"
#include "I2C.h"

void testSequence1(){
  
    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 3; j++ ){
        moveForward(80, 10);
        Serial.println("\n\n\n");
        delay(500);
      }
      
    //turn(-1, 90);//left
    delay(500);  
}
  
  }

  void testSequence2(){

  moveForward(80, 10);
  //delay(500);
  ///turn(-1, 180);
  //delay(500);
  //moveForward(80, 100);
  
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
    case 't': //turn(-1, atoi(&inst[1]));
              break;
    case 'w': moveForward(50, 10);
              break;
    case 'a': //turn(-1, 90);
              break;
    case 's': //turn(1, 90);
              break;
    case 'd': //turn(-1, 180);
              break;
    default:  Serial.println(inst[0]);            
    
    }


}

//-----------------------------------------------
void setup() {
  Serial.begin(115200);
  md.init();
  initI2C();
  //testSequence2();
 
}

void loop() {
  
  // put your main code here, to run repeatedly:
  //Serial.println(getSensorReadingInCM()[1]);
  //Serial.println(getSensorReadingInCM()[2]);
  //straightUsingEncoder();
  turn(1, 90);
  //calibration();
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
	int turningSpeed = 70; //previous value 84
	
    if (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
    {
      while (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
      {
        md.setSpeeds(70, -95);
      }
  
      md.setBrakes(400, 400);
    }
    else
    {
      while (getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR))
      {
        md.setSpeeds(-70, 95);
      }
      md.setBrakes(400, 400);
    }
}

void distanceFromWall(int distance)
{
	//Go to roughly the distance from wall
//  if(getCalibrationReading(frontRightIR) > distance)
//  {
//    while(getCalibrationReading(frontRightIR) > distance)
//      {
//        md.setSpeeds(114, 140);
//      }
//  }
//  else if(getCalibrationReading(frontRightIR) < distance)
//  {
//    while(getCalibrationReading(frontRightIR) < distance)
//      {
//        md.setSpeeds(-114, -140);
//      }
//  }
//  md.setBrakes(400, 400);
//  delay(300);
  
  //Fine tune the distance from wall
  if(getCalibrationReading(frontRightIR) > distance)
  {
    while(getCalibrationReading(frontRightIR) > distance)
      {
        md.setSpeeds(84, 110);
      }
  }
  else if(getCalibrationReading(frontRightIR) < distance)
  {
    while(getCalibrationReading(frontRightIR) < distance)
      {
        md.setSpeeds(-84, -110);
      }
  }
  md.setBrakes(400, 400);
  Serial.println("Set");
  Serial.println(getCalibrationReading(frontRightIR));
}

//Calibration
void calibration()
{
  double threshold = 0.3;
  int wait = 1000;
  int distance = 12;
  
  //Quick calibration against wall
  straighten();
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(12.85);
  delay(wait);
  
  //Fine tune the calibration
  for(int a = 0; a < 3; a++)
  {
    if(abs(getCalibrationReading(frontRightIR) - getCalibrationReading(frontLeftIR)) < threshold)
    {
      break;
    }
    straighten();
    delay(100);
  }
  delay(wait);

  //Fine tune distance from wall
  distanceFromWall(12.85);
  delay(wait);

  //Turn to the left by 90
  turn(-1, 90);
  delay(wait);

  //Move to the distance from wall
  distanceFromWall(12.6);
  delay(wait);

  //Fine tune the calibration
  for(int a = 0; a < 3; a++)
  {
    if(abs(getCalibrationReading(frontRightIR) - getCalibrationReading(frontLeftIR)) < threshold)
    {
      break;
    }
    straighten();
    delay(100);
  }
  delay(wait);

  //Fine tune the distance from wall
  distanceFromWall(12.6);
}
