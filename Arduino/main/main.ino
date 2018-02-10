#include "Motor.h"


//-----------------------------------------------
void setup() {
  Serial.begin(115200);

  md.init();
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

void loop() {
  // put your main code here, to run repeatedly:

}
