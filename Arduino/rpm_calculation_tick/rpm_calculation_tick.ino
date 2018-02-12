#include "DualVNH5019MotorShield.h"
#include <PinChangeInt.h>
//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13
#define singlerevticks 1124

DualVNH5019MotorShield md;

volatile unsigned long Ticks = 0;
unsigned long rpm = 0;
unsigned long elapsed = 0;
unsigned long tStart = micros();
void setup() {
  Serial.begin(115200);
  md.init();
  //PCintPort::attachInterrupt(e1a, &m1Change, CHANGE);
  PCintPort::attachInterrupt(e2a, &m1Change, CHANGE);
  //md.setM1Speed(400);
  md.setM2Speed(400);

  
  unsigned long period = 0.5 * 1000000;  
  for( tStart;  elapsed < period;  ){
    
    elapsed = (micros()-tStart);

    
  }
  
  //PCintPort::detachInterrupt(e1a);
  //md.setM1Speed(0);
  PCintPort::detachInterrupt(e2a);
  md.setM2Speed(0);
  Serial.println("elapsed: ");
  Serial.println(elapsed);
  Serial.println("Ticks: ");
  Serial.println(Ticks);
  rpm = (Ticks*60*2)/1124;
  Serial.println("RPM: ");
  Serial.println(rpm);
  
  
  

}

void m1Change(){
  
  Ticks++;
  /*
  if(Ticks >= singlerevticks){
    //PCintPort::detachInterrupt(e1a);
    //md.setM1Brake(400);
    PCintPort::detachInterrupt(e2a);
    md.setM2Brake(400);
    
    }
    */
  
  }

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(m1Ticks);
  /*
  if(m1Ticks == 1124){
      Serial.println(micros() - tStart);
      m1Ticks++;
    } 
*/
}
