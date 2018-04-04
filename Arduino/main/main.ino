#include "Serial.h"
#include "Forward.h"

void processInst() {

  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  boolean fastestPath = false;
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  int moveCount = 0;
  int delayAmount = 150;
  
  if(instBuff[0] == 'C')
  {
    fastestPath = true;
  }
  
  while (instBuff[index] != ';') 
  {
    switch (instBuff[index]) 
    {
      //-----------------Straight----------------------
      case 'W':   moveCount = instBuff[index + 1] - 48;
            if (moveCount == 1)
              moveForwardOneGridBeta();
            else if(moveCount == 2) 
              moveForwardTwoGridBeta();
            else
              moveForwardBeta(110, 9.7 * moveCount);
            moveForwardNumber = moveCount;
            tooCloseToWall();
            break;
      
      //---------------Turn Left-----------------------     
      case 'A':   acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              delay(delayAmount);
              turnPID(-1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
      
      //--------------Turn Right----------------------
      case 'D':   acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              delay(delayAmount);
              turnPID(1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
      
      //---------------Turn 180-----------------------
      case 'O': acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              turnPID(1, 90);
              delay(delayAmount);
              turnPID(1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
            
      //--------------Corner Calibrate---------------
      case ']':  delay(delayAmount);
            fastCalibration(2);
            acceptTony = false;
            break;
            
      //-------------Calibrate Right Wall------------
      case 'R':   if(acceptTony)
            {
              delay(delayAmount);
              fastCalibration(1);
            }
            break;
            
      //-------------Calibrate Front Wall------------
      case 'F':   delay(delayAmount);
            fastCalibration(0);
            break;
            
      //--------------Initial Calibration--------------
      case '|':   calibration();
            break;
        
      //----------------Sensor Data--------------------
      case 'S':   setOutBuffer('S', getSensorReadingInCM(), 5);
            sendToPi();//Interrupt RPI to notify data is ready
            break;
        
      //--------------Face North-----------------------
      case 'H': delay(300);
        faceNorthCalibration();
        break;
      default:  ;//do nothing
    } 
    index += 2;
  }
    if(fastestPath)
  {
    interruptPi();
  }
  //Serial.print("No. of move inst: ");
  //Serial.println(index-1);
  if (instBuff[0] == 'S') 
  {
    setOutBuffer('S', getSensorReadingInCM(), 7);
    sendToPi();//Interrupt RPI to notify data is ready
  }

}

//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Program Started!!!!");
  md.init();
  pinMode(PI_PIN, OUTPUT);
  digitalWrite(PI_PIN, LOW);
  //initI2C();

  PWM_Mode_Setup();

  int start = millis(); 
  getSensorReadingInCM();


}

void loop(){
  if (dataExist()) {
    processInst();
  }
  if(Serial.available())
  {
    receiveData();
  }
}


