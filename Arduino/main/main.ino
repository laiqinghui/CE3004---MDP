#include "Motor.h"
#include "I2C.h"

void testSequence1(){
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 3; j++ ){
      moveForward(80, 10, true);
      Serial.println("\n\n\n");
      delay(500);
    }
      
     turn(1, 90);//left
    delay(500);  
  }
}

<<<<<<< HEAD
  void testSequence2(){

    calibration();
    
    for(int i = 0; i < 4; i++){
    //moveForward(80, 9.5, true);
    turn(-1, 90);//left
=======
void testSequence2()
{
  for(int i = 0; i < 10; i++){
    moveForward(80, 9.5, true);
>>>>>>> 10bb313b3ed8c9dc814d9a59391f844a9bed34be
    delay(1000);
  }
}

void benTestSequence()
{
  turn(-1, 90);
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
    
    case 'M': moveForward(80, atoi(&inst[1]), true);
              Serial.println("M");    
              break;
    case 'T': turn(-1, atoi(&inst[1]));
              break;
    case 'W': moveForward(80, 10, true);
              break;
    case 'A': turn(-1, 90);
              break;
    case 'D': turn(1, 90);
              break;
    case 'O': turn(-1, 180);
              break;
    case 'S': setOutBuffer(getSensorReadingInCM(), 5);
              interruptPi();//Interrupt RPI to notify data is ready 
              break;
    default:  Serial.println(inst[0]);            
    
    }
    
  resetInBuffer();

}

//-----------------------------------------------
void setup() {
  Serial.begin(115200);
  md.init();
  initI2C();
  //testSequence2();
  //moveForward(80, 95, true);
  benTestSequence();
 
}

void loop() {
  
<<<<<<< HEAD
=======
  // put your main code here, to run repeatedly:  
>>>>>>> 10bb313b3ed8c9dc814d9a59391f844a9bed34be
  if(dataExist()){
    Serial.println("Getting new data...");
    processInst();
    }
    
}
<<<<<<< HEAD




void straighten()
{
	int turningSpeed = 70; //previous value 84
	
    if (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
    {
      while (getCalibrationReading(frontRightIR) > getCalibrationReading(frontLeftIR))
      {
       
        md.setM1Speed(70);
        md.setM2Speed(-95);
      }
  
      md.setBrakes(400, 400);
    }
    else
    {
      while (getCalibrationReading(frontRightIR) < getCalibrationReading(frontLeftIR))
      {
       
        md.setM1Speed(-70);
        md.setM2Speed(95);
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
        md.setM1Speed(84);
        md.setM2Speed(110);
      }
  }
  else if(getCalibrationReading(frontRightIR) < distance)
  {
    while(getCalibrationReading(frontRightIR) < distance)
      {
        md.setM1Speed(-84);
        md.setM2Speed(-110);
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
=======
>>>>>>> 10bb313b3ed8c9dc814d9a59391f844a9bed34be
