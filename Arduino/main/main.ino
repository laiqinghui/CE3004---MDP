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
  delay(1000);
  if(dataExist()){
    Serial.println("Getting new data...");
    processInst();
    resetInBuffer();
    
    }
    
}



/*
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
*/

  
