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
  //turn(1, 180);
  //md.setSpeeds(124, 170);
  calibration();
}

void processInst(){
  
  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  char num[1] = {0};// For checklist
  
  while(instBuff[index] != ';'){ 
    
    switch(instBuff[index]){
      
      case 'W': moveForward(90, 9.5, true);
                break;
      case 'A': turnTemp(-1, 90);
                break;
      case 'D': turnTemp(1, 90);
                break;
      case 'O': turn(-1, 190);
                break;
      case 'c': calibration();
                break;          
      case 'M': moveForward(50, atoi(instBuff+2), true);  
                break;
      case 'T': turn(-1, atoi(instBuff+2));
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
    
    
    if(instBuff[++index] != ';')
      delay(200);
    
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

  //benTestSequence();

}

void loop() 
{
  Serial.println((int)getSensorReadingInCM()[3]);
  delay(500);
  if(dataExist()){
    delay(100);//Delay for ack packet to be sent out. To allow RPI to request and recieve data before we start moving which will affect interrupt operations 
    processInst();
    }
    
}


