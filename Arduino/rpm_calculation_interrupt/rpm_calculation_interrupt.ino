#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

DualVNH5019MotorShield md;

int squareWidth;
//int sqWidths[300] = {0};
int counter = 1;
unsigned long elapsed = 0;
double dataAquireStartTime = 0.2 * 1000000;// Only process encoder data after this amt of time (micro seconds)
double dataAquireEndTime = 0.3 * 1000000;// Only process encoder data before this amt of time (micro seconds)
volatile int pwm_value = 0;
volatile int prev_time = 0;
uint8_t latest_interrupted_pin;
 
void rising()
{

  squareWidth = micros()- prev_time;
  //sqWidths[counter] = squareWidth;

  if((elapsed > dataAquireStartTime) && (elapsed < dataAquireEndTime))
    Serial.println(squareWidth);
  //Serial.println(",");
  counter++;
  prev_time = micros();
}
 
 
void setup() {
  //pinMode(e1a, INPUT); digitalWrite(e1a, LOW);
  Serial.begin(115200);
  md.init();

  unsigned long period = 0.5 * 1000000;       // 0.5 seconds
  unsigned long tStart = micros();

  Serial.println("===========START=================");
  md.setM1Speed(350);
  //md.setM2Speed(350);//M2 min speed is 80
  PCintPort::attachInterrupt(e1a, &rising, RISING);
  //PCintPort::attachInterrupt(e2a, &rising, RISING);
  for( tStart;  elapsed < period;  ){

    elapsed = (micros()-tStart);
    
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


