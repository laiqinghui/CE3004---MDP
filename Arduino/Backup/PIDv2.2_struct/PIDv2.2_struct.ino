#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//------------Other constants and declarations----------
#define Pi 3.1416
#define singlerevticks 2248.86

signed long wheelDiameter = 6*Pi;
signed long ticksPerCM = singlerevticks/wheelDiameter;

//------------PID Structs------------
struct MotorPID {
  float prevTuneSpeed;
  float currentErr;
  float prevErr1;
};

//------------Interrupt declarations------------
volatile signed long M1Ticks = 0;

volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;

volatile int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;
volatile signed long entry_time_M2 = 0; 

//ISR for Motor1(Right) encoder 
void risingM1()
{
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1Ticks +=4;
}

//ISR for Motor2(Left) encoder
void risingM2()
{
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
}



signed long sqWidthToRPM(int sqWidth){

  if(sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
  signed long sqwavesOneS = 1000000/sqWidth;//1/(sqWidth/1000000)
  signed long sqwavesOneM = sqwavesOneS*60;
  signed long revPerMin = sqwavesOneM/sqwavesPerRev;

  return revPerMin;
 
  }


void tuneM1(int desiredRPM, MotorPID *M1){


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);
  
  Serial.print("M1 Current RPM: ");
  Serial.println(currentRPM);
  
  
  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.5*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr + (0.47/0.05)*(M1->currentErr - M1->prevErr1);
  /*
  Serial.print("currentErr_M1 ");
  Serial.println(currentErr_M1);
  Serial.print("M1 tuneSpeed ");
  Serial.println(tuneSpeed);
  Serial.println();
  */
  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

 
  }

  void tuneM2(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
  Serial.print("M2 Current RPM: ");
  Serial.println(currentRPM);
  
  
  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr + (0.5/0.05)*(M2->currentErr - M2->prevErr1);
  /*
  Serial.print("currentErr_M2 ");
  Serial.println(currentErr_M2);
  Serial.print("M2 tuneSpeed ");
  Serial.println(tuneSpeed);
  Serial.println();
  */
  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;
  
 
  }


void moveForward(int rpm, int distance){

   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = 0*rpm;
   
   //unsigned int rpm1 = 0;
   //unsigned int rpm2 = 0;


    
    MotorPID M1pid = {184, 0, 0};
    MotorPID M2pid = {219, 0, 0};
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    md.setM1Speed(184);
    delay(1);
    md.setM2Speed(219);

    
  

    while(M1Ticks < distanceTicks){
      /*
      rpm1 = sqWidthToRPM(squareWidth_M1);
      rpm2 = sqWidthToRPM(squareWidth_M2);
      
      Serial.print("M1 Current RPM: ");
      Serial.println(rpm1);
      Serial.print("\nM2 Current RPM: ");
      Serial.println(rpm2);
      */
      if(sqWidthToRPM(squareWidth_M2) > pidStartRPM || sqWidthToRPM(squareWidth_M1) > pidStartRPM){
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 
          //Serial.print("Tune interval: ");
          //Serial.println(interval); 
          tuneM1(rpm, &M1pid);
          tuneM2(rpm, &M2pid);
          tuneExitTime = micros();
          
        }
      }
    }
      md.setBrakes(400,400);
      delay(1000);
      md.setSpeeds(-230.839, -265.47);
      delay(7000);
      md.setBrakes(400,400);
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  md.init();
  moveForward(80, 150);
}

void loop() {
  // put your main code here, to run repeatedly:

}





  
      
    
    
    
