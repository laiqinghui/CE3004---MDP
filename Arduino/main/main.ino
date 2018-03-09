#include "I2C.h"
#include "Calibration.h"
#include "MotorBeta.h"


void benTestSequence()
{
  //straightenTune();
  //moveForward(80, 30, true);  
  //turnPID(-1, 90);
  //md.setSpeeds(124, 170);
  //fastCalibration(2);
  //delay(1000);
  //moveForward(80, 9.5, true);
  //benForward(80, 9.5, true);
  //moveForwardOneGrid(80);
  //turnTest(-1, 90);
}

void processInst(){
  
  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  char num[1] = {0};// For checklist
  int moveCount = 0;
  int delayAmount = 300;
  
  while(instBuff[index] != ';'){ 
    delay(delayAmount);
    switch(instBuff[index]){
      
      case 'W': moveCount = instBuff[index+1] - 48;
                if(moveCount == 1)
                  //benForward(80, 9.5, true);
                 moveForwardOneGrid(60);
                else moveForward(70, 9.5*moveCount, true);
                
                break;
      case 'A': Serial.println('A');
                moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  
                  turnTest(-1, 90);
                    delay(delayAmount);
                }
                break;
      case 'D': moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  turnTest(1, 90);
                  delay(delayAmount);
                }
                break;
      case 'O': moveCount = instBuff[index+1] - 48;
                for(int a = 0; a < moveCount; a++){
                  turnTest(1, 180);
                  delay(delayAmount);  
                }
                break;
      case ']': //PWM_Mode_Setup();
                fastCalibration(2);
                break;
      case 'R': fastCalibration(1);
                break;
      case 'F': fastCalibration(0);
                break;          
      case '|': calibration();
                break;                     
      case 'M': moveForward(50, atoi(instBuff+2), true);  
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
    Serial.print("No. of move inst: ");
    Serial.println(index-1);
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

  benTestSequence();
}

void loop() 
{
  //PWM_Mode_Setup();
  //getUltraSound2Reading();
  //Serial.println("Set");
  //Serial.println(getCalibrationReading(false)[0]);
  getUltraSoundDistance();
  getUltraSoundDistance();
  getUltraSoundDistance();
  
  if(dataExist()){
    //delay(100);//Delay for ack packet to be sent out. To allow RPI to request and recieve data before we start moving which will affect interrupt operations 
    processInst();
    }
    
}


