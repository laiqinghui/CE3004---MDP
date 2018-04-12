#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"
#include "Calibration.h"

//----------------------------------------------------------------FORWARD----------------------------------------------------------------
void moveForwardBeta(int rpm, double distance);
void calibrateBeforeMoveForward();

void moveForwardBeta(int rpm, double distance) {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  double distanceTicks = 1.01 * distance * ticksPerCM;
  unsigned int currentTicksM1 = 0;
  //unsigned long currentTicksM2 = 0;
  
  breakTicks = distanceTicks;

  int m1setSpeed = 350;
  int m2setSpeed = 360;

  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();

  MotorPID M1pid = {m1setSpeed, 0, 0, 0.113};//0.1=>50
  MotorPID M2pid = {m2setSpeed, 0, 0, 0.1};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms //was 0.125
  
  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);
  setTicks(0, 0);

  md.setSpeeds(m1setSpeed, m2setSpeed);  

  while (!movementDone)
  {
    noInterrupts();
    currentTicksM1 = M1ticks;
    interrupts();

    tuneEntryTime = micros();
    interval = tuneEntryTime - tuneExitTime;
    
    if (interval >= 5000)
    {
      tuneMotors(rpm, &M1pid, &M2pid);
      /*
      //Gradual breaking
      if(currentTicksM1 > 0.9*distanceTicks){
        tuneMotors(rpm*0.7, &M1pid, &M2pid);
        
      }
      else
      {
        tuneMotors(rpm, &M1pid, &M2pid);
      }
      */
      tuneExitTime = micros();
    }

  }//end of while

 

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  breakTicks = 0;
  movementDone = false;
  setTicks(0, 0);
  setSqWidth(0, 0);
}

void moveForwardOneGridBeta() {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  signed int currentTicksM1 = 0;
  signed int currentTicksM2 = 0;

  
  int m1setSpeed = 300;//SETPOINT TARGET //250
  int m2setSpeed = 310; //265
  int tuneSpeedM2 = 0;
  
  
  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();
  breakTicks = 1.015 * 9.5 * ticksPerCM;//0.95
  MotorPID M2 = {m2setSpeed , 0, 0, 0.400}; //
  enableInterrupt( e1a, dummy, RISING);
  enableInterrupt( e2b, dummy, RISING);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  
  setTicks(0,0);

   
      while(!movementDone)
      {
        tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        
        if(interval >= 5000)
        { 
          
          noInterrupts();
          currentTicksM1 = M1ticks;
          currentTicksM2 = M2ticks;
          interrupts();
      
          M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
          tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.01)*(M2.currentErr - M2.prevErr1);
          if(!movementDone){
            if(tuneSpeedM2 > 400)
              OCR1B = 400;
            else 
              OCR1B = tuneSpeedM2;
            
            }
            
          /*
          Serial.print("tuneSpeedM2: ");
          Serial.println(tuneSpeedM2);
          Serial.println(currentTicksM2);
          */
          M2.prevTuneSpeed = tuneSpeedM2;
          M2.prevErr1 = M2.currentErr;
          tuneExitTime = micros();
        
        }//end of if

        
         
        }// end of while

      
          



  disableInterrupt(e1a);
  disableInterrupt(e2b);
  //Serial.print("breakTicksM2: ");
  //Serial.println(M2ticks);
  //Serial.print("breakTicksM1: ");
  //Serial.println(M1ticks);
  breakTicks = 0;
  movementDone = false;
 
  setTicks(0, 0);
  setSqWidth(0, 0);
}

void moveForwardTwoGridBeta() {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  signed int currentTicksM1 = 0;
  signed int currentTicksM2 = 0;

  
  int m1setSpeed = 300;//SETPOINT TARGET //250
  int m2setSpeed = 310; //265
  int tuneSpeedM2 = 0;
  
  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();
  breakTicks = 1.03 * 19 * ticksPerCM;//0.95
  MotorPID M2 = {m2setSpeed , 0, 0, 0.40}; //
  enableInterrupt( e1a, dummy, RISING);
  enableInterrupt( e2b, dummy, RISING);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  
  setTicks(0,0);

   
      while(!movementDone)
      {
        tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        
        if(interval >= 5000)
        { 
          
          noInterrupts();
          currentTicksM1 = M1ticks;
          currentTicksM2 = M2ticks;
          interrupts();
      
          M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
          tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.01)*(M2.currentErr - M2.prevErr1);
          if(!movementDone){
            if(tuneSpeedM2 > 400)
              OCR1B = 400;
            else 
              OCR1B = tuneSpeedM2;
            }
            
          /*
          Serial.print("tuneSpeedM2: ");
          Serial.println(tuneSpeedM2);
          Serial.println(currentTicksM2);
          */
          M2.prevTuneSpeed = tuneSpeedM2;
          M2.prevErr1 = M2.currentErr;
          tuneExitTime = micros();
        
        }//end of if

        
       

         
        }// end of while

      
          


      
    //Serial.println(M2.prevTuneSpeed);

  disableInterrupt(e1a);
  disableInterrupt(e2b);
      //Serial.print("breakTicksM2: ");
    //Serial.println(M2ticks);
    //Serial.print("breakTicksM1: ");
    //Serial.println(M1ticks);
  breakTicks = 0;
  movementDone = false;

  
 
  setTicks(0, 0);
  setSqWidth(0, 0);
}


void moveForwardMultipleGridBeta(int moveCount) {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  signed int currentTicksM1 = 0;
  signed int currentTicksM2 = 0;

  
  int m1setSpeed = 380;//SETPOINT TARGET //250
  int m2setSpeed = 400; //265
  int tuneSpeedM2 = 0;
  
  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();
  if(moveCount >= 6)
    breakTicks = 0.99 * (moveCount*10) * ticksPerCM;
  else 
    breakTicks = 0.97 * (moveCount*10) * ticksPerCM;
       
  MotorPID M2 = {m2setSpeed , 0, 0, 0.35}; //40
  enableInterrupt( e1a, dummy, RISING);
  enableInterrupt( e2b, dummy, RISING);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  
  setTicks(0,0);

   
      while(!movementDone)
      {
        tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        
        if(interval >= 5000)
        { 
          
          noInterrupts();
          currentTicksM1 = M1ticks;
          currentTicksM2 = M2ticks;
          interrupts();
      
          M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
          tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.01)*(M2.currentErr - M2.prevErr1);
          if(!movementDone){
            if(tuneSpeedM2 > 400)
              OCR1B = 400;
            else 
              OCR1B = tuneSpeedM2;
          }
          /*
          Serial.print("tuneSpeedM2: ");
          Serial.println(tuneSpeedM2);
          Serial.println(currentTicksM2);
          */
          M2.prevTuneSpeed = tuneSpeedM2;
          M2.prevErr1 = M2.currentErr;
          tuneExitTime = micros();
        
        }//end of if

        
       

         
        }// end of while

        Serial.print("BreakTicksM2: ");
        Serial.println(currentTicksM2);
        Serial.print("BreakTicksM1: ");
        Serial.println(currentTicksM1);
      
          


      
    //Serial.println(M2.prevTuneSpeed);

  disableInterrupt(e1a);
  disableInterrupt(e2b);
      //Serial.print("breakTicksM2: ");
    //Serial.println(M2ticks);
    //Serial.print("breakTicksM1: ");
    //Serial.println(M1ticks);
  breakTicks = 0;
  movementDone = false;

  
 
  setTicks(0, 0);
  setSqWidth(0, 0);
}
