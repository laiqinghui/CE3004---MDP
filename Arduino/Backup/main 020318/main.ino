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
      
     turn(-1, 90);//left
    delay(500);  
  }
}


void testSequence2()
{
  for(int i = 0; i < 10; i++){
    moveForward(80, 9.5, true);
    delay(1000);
    Serial.println("\n\n");
  }
}

void benTestSequence()
{
  //turn(1, 720);
  //md.setSpeeds(124, 170);
  calibration();
}

void processInst(){
  
  char *buff = getinBuffer();
  char inst[5]  = {0}; 
  int index = 0;
  int i = 1;
  
  delay(1000);//Delay for ack packet to be sent out. To allow RPI to request and recieve data before we start moving which will affect interrupt operations 
  
  while(buff[index] != ';'){
    inst[index] = buff[index];
    index++;
  }
  

  while(i < index){ 
    
  
    switch(inst[i]){
      
  
      case 'W': moveForward(50, 9.5, true);
                break;
      case 'A': turn(-1, 90);
                break;
      case 'D': turn(1, 90);
                break;
      case 'O': turn(-1, 180);
                break;
      case 'c': calibration();
                break;          
      case 'M': moveForward(50, atoi(&inst[2]), true);  
                break;
      case 'T': turn(-1, atoi(&inst[2]));
                break;
      case 'S': setOutBuffer('S', getSensorReadingInCM(), 5);
                interruptPi();//Interrupt RPI to notify data is ready 
                break;
      default:  ;//do nothing            
      
      }
  
    i++;
    
    }
    if(inst[0] == 'S'){
      delay(100);//Stop for awhile before we check sensor reading
      setOutBuffer('S', getSensorReadingInCM(), 5);
      printArray(outBuffer, 6);
      interruptPi();//Interrupt RPI to notify data is ready 
      }
    

}

//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Program Started!");
  md.init();
  initI2C();
}

void loop() {
 
  if(dataExist()){
    processInst();
    }
    
}


