#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

DualVNH5019MotorShield md;

volatile int squareWidth;
volatile unsigned long elapsed = 0;
volatile unsigned long tStart = micros();
volatile unsigned long prev_time = 0;
volatile unsigned long entry_time = 0;
volatile unsigned long recordTime = 0;
volatile unsigned long interruptedCounter = 0;
volatile boolean recorded = true;

double dataAquireStartTime = 0.2 * 1000000;// Only process encoder data after this amt of time (micro seconds)
double dataAquireEndTime = 0.3 * 1000000;// Only process encoder data before this amt of time (micro seconds)
int recordedCounter = 0;
uint8_t latest_interrupted_pin;

 
void rising()
{
  entry_time = micros();
  if(recorded){
    squareWidth = entry_time - prev_time;
    //recordTime = elapsed;
    recorded = false;
  }
  interruptedCounter++; 
  prev_time = micros();
}
 
 
void setup() {
  
  Serial.begin(115200);
  md.init();

  unsigned long period = 0.5 * 1000000;       // 0.5 seconds
  

  Serial.println("===========START=================");
  md.setM1Speed(400);
  //md.setM2Speed(400);//M2 min speed is 80
  PCintPort::attachInterrupt(e1a, &rising, RISING);
  //PCintPort::attachInterrupt(e2a, &rising, RISING);
  for( tStart;  elapsed < period;  ){

    elapsed = (micros()-tStart);

     if(!recorded && ((elapsed > dataAquireStartTime) && (elapsed < dataAquireEndTime)) ){
        //Serial.print(elapsed);
        //Serial.print(" ");
        Serial.println(squareWidth);
        recordedCounter++;
        recorded = true;
      }
    
    
  }
  PCintPort::detachInterrupt(e1a);
  //PCintPort::detachInterrupt(e2a);
  md.setM1Speed(0);
  //md.setM2Speed(0);
  
  Serial.println("===========End=================");
  Serial.print("Duration: ");
  Serial.println(elapsed);
  elapsed = 0;//reset elapsed
  Serial.println("\n\n");
  
 
}
 
void loop() { }


