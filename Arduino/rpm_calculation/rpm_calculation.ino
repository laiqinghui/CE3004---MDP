#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DualVNH5019MotorShield.h"

//OLED def
#define OLED_RESET 4

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

unsigned long durationH;
unsigned long durationL;
unsigned long durationHLAvg;
int squareWidth;
int sqWidths[200] = {0};
Adafruit_SSD1306 display(OLED_RESET);
DualVNH5019MotorShield md;


void setup() {
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  Serial.println("Dual VNH5019 Motor Shield");
  
  md.init();
  

  md.setM1Speed(50);
 
  Serial.println("===========START=================");
  unsigned long period = 5 * 1000;       // 5 seconds
  unsigned long elapsed = 0;
  unsigned long tStart = millis();
  int counter = 1;
  

  
  for( tStart;  elapsed < period;  ){

    durationH = pulseIn(e1a, HIGH);
    durationL = pulseIn(e1a, LOW);
    //Serial.print("PulseWidth High = ");
    //Serial.println(durationH);
    //Serial.print("PulseWidth Low = ");
    //Serial.println(durationL);
    //durationHLAvg = (durationH+durationL)/2;
    //Serial.print("PulseWidth Avg = ");
    //Serial.println(durationHLAvg);
    //squareWidth = durationHLAvg*2;
    squareWidth = durationH+durationL;
    //Serial.print("SquareWidth = ");
    //Serial.println(squareWidth);
    //Serial.println();
    sqWidths[counter] = squareWidth;
    counter++;
    elapsed = (millis()-tStart);
    //Serial.println("Running....");
  }
  md.setM1Speed(0);
  Serial.println("===========End=================");
  Serial.print("Duration: ");
  Serial.println((elapsed - tStart));
  Serial.println("\n\n");
  printSqWaves(counter);


}

void loop() {


 


}

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

