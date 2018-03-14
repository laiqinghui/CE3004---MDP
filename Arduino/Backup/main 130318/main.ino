#include "I2C.h"
#include "Forward.h"


void benTestSequence()
{
  //straightenTune();
  //moveForward(80, 30, true);  
  //turnPID(-1, 90);
  //md.setSpeeds(124, 170);
  //while(true)
  //{
   //fastCalibration(2);
  //}
  //delay(1000);
  //moveForward(80, 9.5, true);
  //benForward(80, 9.5, true);
  //moveForwardOneGrid(90);
  //turnTest(-1, 90);
  //test();
}

void processInst(){
  
  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  char num[1] = {0};// For checklist
  int moveCount = 0;
  int delayAmount = 200;
  
  while(instBuff[index] != ';'){ 
    switch(instBuff[index]){
      
      case 'W': moveCount = instBuff[index+1] - 48;
                //delay(delayAmount);
                if(moveCount == 1)
                 moveForwardOneGrid(90);
                else //moveForward(90, 9.5*moveCount, true);
                     moveForwardBeta(90, 9.7*moveCount);
                break;
      case 'A': Serial.println('A');
                moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  //turnPID(-1, 90);
                  turn90PIDBeta(-1);
                  delay(delayAmount);
                }
                //Reset side wall reading
                resetSideWall();
                break;
      case 'D': moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  //turnPID(1, 90);
                  turn90PIDBeta(1);
                  delay(delayAmount);
                }
                //Reset side wall reading
                resetSideWall();
                break;
      case 'O': moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  turnPID(1, 90);
                  delay(delayAmount);
                  turnPID(1, 90);  
                  delay(delayAmount);
                }
                //Reset side wall reading
                resetSideWall();
                break;
      case ']': fastCalibration(2);
                break;
      case 'R': fastCalibration(1);
                break;
      case 'F': fastCalibration(0);
                break;          
      case '|': calibration();
                break;                     
      case 'T': turnPID(-1, atoi(instBuff+2));
                break;
      case 'S': setOutBuffer('S', getSensorReadingInCM(), 5);
                interruptPi();//Interrupt RPI to notify data is ready 
                break;
      case 'I': num[0] = atoi(instBuff+2) +1;
                Serial.print("num = ");
                Serial.println(num[0]);
                setOutBuffer('S', num, 1);
                interruptPi();
                break;
      default:  ;//do nothing            
      
      }
    
    index+=2;
    //if(instBuff[index] != ';')
      //delay(200);
    
    }
    //Serial.print("No. of move inst: ");
    //Serial.println(index-1);
    if(instBuff[0] == 'S'){
  
      setOutBuffer('S', getSensorReadingInCM(), 5);
      printArray(outBuffer, 6);
      interruptPi();//Interrupt RPI to notify data is ready 
      }
    

}

//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Program Started!!!!");
  md.init();
  initI2C();

  /*
  EEPROM.write(0, 0);
  EEPROM.write(1, 0);
  EEPROM.write(2, 0);
  */
  
  turnRight90Offset = EEPROM.read(1);
  turnLeft90Offset = EEPROM.read(2);

  if(turnRight90Offset > 127)
  {
    turnRight90Offset = (turnRight90Offset - 256);
  }
  if(turnLeft90Offset > 127)
  {
    turnLeft90Offset = (turnLeft90Offset - 256);
  }
  
  //benTestSequence();
}

void loop() 
{
  //PWM_Mode_Setup();
  //getUltraSound2Reading();
  //Serial.println("Set");
  //Serial.println(getIRSensorReading()[1]);
    
  if(dataExist()){
    //delay(100);//Delay for ack packet to be sent out. To allow RPI to request and recieve data before we start moving which will affect interrupt operations 
    processInst();
  }
    
}


