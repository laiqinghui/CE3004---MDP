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
  double distanceTicks = 1.02 * distance * ticksPerCM;
  unsigned int currentTicksM1 = 0;
  //unsigned long currentTicksM2 = 0;
  
  breakTicks = distanceTicks;

  int m1setSpeed = 300;
  int m2setSpeed = 290;

  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();

  MotorPID M1pid = {m1setSpeed, 0, 0, 0.102};//0.1=>50
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
      
      //Gradual breaking
      if(currentTicksM1 > 0.6*distanceTicks){
        tuneMotors(rpm*0.7, &M1pid, &M2pid);
        
      }
      else
      {
        tuneMotors(rpm, &M1pid, &M2pid);
      }
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

  
  int m1setSpeed = 250;//SETPOINT TARGET //250
  int m2setSpeed = 265; //265
  int tuneSpeedM2 = 0;
  int moveSet = 0;
  
  
  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();
  breakTicks = 0.99 * 9.5 * ticksPerCM;//0.95
  MotorPID M2 = {m2setSpeed , 0, 0, 0.400}; //
  enableInterrupt( e1a, dummy, RISING);
  enableInterrupt( e2b, dummy, RISING);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  
  setTicks(0,0);
  boolean brakesPending = true;

   
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
          if(!movementDone)
            OCR1B = tuneSpeedM2;
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
  int m2setSpeed = 315; //265
  int tuneSpeedM2 = 0;
  int moveSet = 0;
  
  
  //Check using right side sensor if need to calibrate
  //calibrateBeforeMoveForward();
  breakTicks = 1 * 19 * ticksPerCM;//0.95
  MotorPID M2 = {m2setSpeed , 0, 0, 0.40}; //
  enableInterrupt( e1a, dummy, RISING);
  enableInterrupt( e2b, dummy, RISING);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  
  setTicks(0,0);
  boolean brakesPending = true;

   
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
          if(!movementDone)
            OCR1B = tuneSpeedM2;
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

