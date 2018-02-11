#include "Motor.h"
#include "I2C.h"


//-----------------------------------------------
void setup() {
  Serial.begin(115200);
  md.init();
  initI2C();
  
 
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if(dataExist()){
    Serial.println("Getting new data...");
    processInst();
    resetInBuffer();
    
    }
    
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

  
