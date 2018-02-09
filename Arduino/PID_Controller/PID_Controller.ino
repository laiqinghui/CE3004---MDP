#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED def
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

DualVNH5019MotorShield md;

//------------PID declarations------------
#define singlerevticks 2248
#define sampleInterval 0.005
//Motor 1(Right) constant
#define td1 0.00496
#define ts1 0.09667 //Orig: 0.09667, tune: 0.08667 //Higher denotes more gain on correction
#define k1 0.35093

//Motor 2(Left) constant
#define td2 0.00264
#define ts2 0.0995 //Orig: 0.05827, tune: 0.09827 
#define k2 0.31323

//Motor 1(Right) PID parameters
unsigned long kc1 = (1.2*ts1)/(k1*td1);
unsigned long ti1 = 2*td1;
unsigned long kp1 = kc1;
unsigned long ki1 = kc1/ti1;
unsigned long kd1 = kc1*td1;

unsigned long k1_1 = kp1+ki1+kd1;
unsigned long k2_1 = (-kp1-2)*kd1;
unsigned long k3_1 = kd1;

//Motor 2(left) PID parameters
unsigned long kc2 = (1.2*ts2)/(k2*td2);
unsigned long ti2 = 2*td2;
unsigned long kp2 = kc2;
unsigned long ki2 = kc2/ti2;
unsigned long kd2 = kc2*td2;

unsigned long k1_2 = kp2+ki2+kd2;
unsigned long k2_2 = (-kp2-2)*kd2;
unsigned long k3_2 = kd2;


unsigned long currentErr_M1 = 0;
unsigned long prevErr1_M1 = 0;
unsigned long prevErr2_M1 = 0;

unsigned long currentErr_M2 = 0;
unsigned long prevErr1_M2 = 0;
unsigned long prevErr2_M2 = 0;



//------------Interrupt declarations------------
unsigned long startCaptureTime = 0.55 * 1000000;       // 0.1 seconds for free standing

volatile int squareWidth_M1 = 0;
volatile unsigned long prev_time_M1 = 0;
volatile unsigned long entry_time_M1 = 0;
volatile unsigned long ticks = 0;

volatile int squareWidth_M2 = 0;
volatile unsigned long prev_time_M2 = 0;
volatile unsigned long entry_time_M2 = 0;

void risingM1()
{
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  //Serial.println(squareWidth_M1);
  prev_time_M1 = micros();
  ticks +=4;
}

void risingM2()
{
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = micros();
}

//----------------------------------------------

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  md.init();
  moveForward(100, 5);
  
}

void loop() {

}

void moveForward(int rpm, int distance){

   unsigned long tStart = micros();
   unsigned long tuneEntryTime = 0;
   unsigned long tuneExitTime = 0;
   unsigned long interval = 0;
   unsigned long distanceTicks = singlerevticks * 8;
   int pidStartRPM = rpm/4;  //rpm/2
   boolean startPID = false;
   
   PCintPort::attachInterrupt(e1a, &risingM1, RISING);
   PCintPort::attachInterrupt(e2a, &risingM2, RISING);

   //Must start with a offset to reduce acceleration difference
   
   //md.setM1Speed(342);//320
   //md.setM2Speed(342.698);//342.698
   //md.setSpeeds(296, 342); 

/*
   for(int i = 1; i < 300; i++ ){
    
      if(i+4 < 300)
        md.setM1Speed(i+4);
      md.setM2Speed(i);
    
    }
    */
    md.setM1Speed(300);
    //delay(1.5);
    md.setM2Speed(300);
   
   while(ticks < distanceTicks){//18432
      
      if(!startPID ){
        
        if( sqWidthToRPM(squareWidth_M1) >= pidStartRPM ||  sqWidthToRPM(squareWidth_M2) >= pidStartRPM){ //(micros() - tStart) > startCaptureTime
          
          Serial.print("PID Starts");
          Serial.print("M1 RPM: ");
          Serial.println(sqWidthToRPM(squareWidth_M1));
          Serial.print("M2 RPM2: ");
          Serial.println(sqWidthToRPM(squareWidth_M2));
          startPID = true;
          
        }else{
          
          Serial.print("M1 Current RPM: ");
          Serial.println(sqWidthToRPM(squareWidth_M1));
          Serial.print("M2 Current RPM: ");
          Serial.println(sqWidthToRPM(squareWidth_M2));
          Serial.println();
          
          } 
      }else {
        
        tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 
          Serial.print("Tune interval: ");
          Serial.println(interval); 
          tuneM2(rpm);
          tuneM1(rpm);
          tuneExitTime = micros();
        }
        
      }

    }

    md.setM1Brake(300);
    md.setM2Brake(300);
    delay(500);
    ticks = 0;
    md.setM2Speed(-342.698);//342.698
    md.setM1Speed(-280);//296.216

    while(ticks < distanceTicks);
    md.setM1Brake(250);
    md.setM2Brake(300);
     
   
  }

void tuneM1(int desiredRPM){

  
  double tuneSpeed;
  double currentRPM = sqWidthToRPM(squareWidth_M1);
  Serial.print("M1 Current RPM: ");
  Serial.println(currentRPM);
  Serial.println();
  currentErr_M1 =  desiredRPM - currentRPM;
  tuneSpeed = currentRPM + k1_1*currentErr_M1 + k2_1*prevErr1_M1 + k3_1*prevErr2_M1;
  //Serial.print("M1 tuneSpeed ");
  //Serial.println(tuneSpeed);
  md.setM1Speed(tuneSpeed);

  prevErr2_M1 = prevErr1_M1;
  prevErr1_M1 = currentErr_M1;

  
  }

 void tuneM2(int desiredRPM){

  
  double tuneSpeed;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  Serial.print("M2 Current RPM: ");
  Serial.println(currentRPM);
  
  currentErr_M2 =  desiredRPM - currentRPM;
  tuneSpeed = currentRPM + k1_2*currentErr_M2 + k2_2*prevErr1_M2 + k3_2*prevErr2_M2;
  //Serial.print("M2 tuneSpeed ");
  //Serial.println(tuneSpeed);
  md.setM2Speed(tuneSpeed);

  prevErr2_M2 = prevErr1_M2;
  prevErr1_M2 = currentErr_M2;

  
  }  
   

unsigned long sqWidthToRPM(int sqWidth){

  if(sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
  unsigned long sqwavesOneS = 1000000/sqWidth;//1/(sqWidth/1000000)
  unsigned long sqwavesOneM = sqwavesOneS*60;
  unsigned long revPerMin = sqwavesOneM/sqwavesPerRev;

  return revPerMin;
 
  }
