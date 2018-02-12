#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

DualVNH5019MotorShield md;

int squareWidth;
int sqWidths[300] = {0};
int counter = 1;

volatile int pwm_value = 0;
volatile int prev_time = 0;
uint8_t latest_interrupted_pin;
 
void rising()
{
  latest_interrupted_pin=PCintPort::arduinoPin;
 // PCintPort::attachInterrupt(latest_interrupted_pin, &falling, FALLING);
  squareWidth = micros()- prev_time;
  sqWidths[counter] = squareWidth;
  Serial.println(counter);
  counter++;
  
  prev_time = micros();
}
 
void falling() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  PCintPort::attachInterrupt(latest_interrupted_pin, &rising, RISING);
  pwm_value = micros()-prev_time;
  Serial.println(pwm_value);
}
 
void setup() {
  //pinMode(e1a, INPUT); digitalWrite(e1a, LOW);
  Serial.begin(115200);
  PCintPort::attachInterrupt(e1a, &rising, RISING);
  md.init();

  unsigned long period = 1 * 1000000;       // 5 seconds
  unsigned long elapsed = 0;
  unsigned long tStart = micros();

  Serial.println("===========START=================");
  md.setM1Speed(100);

  for( tStart;  elapsed < period;  ){

    elapsed = (micros()-tStart);
    
  }
  PCintPort::detachInterrupt(e1a);
  md.setM1Speed(0);
  Serial.println("===========End=================");
  Serial.print("Duration: ");
  Serial.println((elapsed - tStart));
  Serial.println("\n\n");
  
  printSqWaves(counter);
}
 
void loop() { }

void printSqWaves(int sizeOfArray){
    Serial.print("sizeOfArray = ");
    Serial.println(sizeOfArray);
    int arrayindex = 1;
    while(arrayindex <= sizeOfArray){
      
        Serial.print("SquareWidth = ");
        Serial.println(sqWidths[arrayindex]);
        arrayindex++;
      
      }
  
  }
