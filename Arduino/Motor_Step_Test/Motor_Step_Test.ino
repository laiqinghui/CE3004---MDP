#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"


//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

DualVNH5019MotorShield md;

volatile int squareWidth;
volatile unsigned long prev_time = 0;
volatile unsigned long entry_time = 0;
volatile unsigned long tStart = micros();
volatile unsigned long recordTime = 0;
volatile unsigned long interruptedCounter = 0;
volatile unsigned long elapsed = 0;
volatile boolean recorded = true;

boolean stepped = false;
double dataAquireStartTime = 0.2 * 1000000;// Only process encoder data after this amt of time (micro seconds)
double dataAquireEndTime = 0.3 * 1000000;// Only process encoder data before this amt of time (micro seconds)
int recordedCounter = 0;
uint8_t latest_interrupted_pin;



 
void rising()
{
  
  entry_time = micros();
  if(recorded){
    squareWidth = entry_time - prev_time;
    recordTime = elapsed - tStart;
    recorded = false;
  }
  interruptedCounter++; 
  prev_time = micros();
  
  /*
  entry_time = micros();
  squareWidth = entry_time - prev_time;
  Serial.println(squareWidth);
  prev_time = micros();
  */
}

void setup() {
  Serial.begin(115200);
  md.init();

  unsigned long period = 1.9 * 1000000;       // 1.9 seconds
  unsigned long steppedTime = 1.4 * 1000000;       // 1.4 seconds
  

  Serial.println("===========START=================");
  //md.setM1Speed(350);
  md.setM2Speed(350);//M2 min speed is 80
  //PCintPort::attachInterrupt(e1a, &rising, RISING);
  PCintPort::attachInterrupt(e2a, &rising, RISING);
  for( tStart;  elapsed < period;  ){
    
    elapsed = (micros()-tStart);

    
    if(!recorded){
        Serial.print(recordTime);
        Serial.print(" ");
        Serial.println(squareWidth);
        recordedCounter++;
        recorded = true;
      }
    
    if(!stepped && elapsed > steppedTime){
       //md.setM1Speed(300);
       md.setM2Speed(300);
       Serial.print("Stepped time: ");
       Serial.println(elapsed);
       stepped = true;
      }
      
    
  }
  //PCintPort::detachInterrupt(e1a);
  PCintPort::detachInterrupt(e2a);
  //md.setM1Speed(0);
  md.setM2Speed(0);
  
  Serial.println("===========End=================");
  Serial.print("Total Duration: ");
  Serial.println(elapsed);
  Serial.print("No. of interrupts: ");
  Serial.println(interruptedCounter);
  Serial.print("No. of recordings: ");
  Serial.println(recordedCounter);
  elapsed = 0;//reset elapsed
  Serial.println("\n\n");

}

void loop() {
  // put your main code here, to run repeatedly:

}
