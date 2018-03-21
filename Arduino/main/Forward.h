#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"
#include "Calibration.h"

//----------------------------------------------------------------FORWARD----------------------------------------------------------------
void moveForwardOneGrid(int rpm);
void moveForwardBeta(int rpm, double distance);
void calibrateBeforeMoveForward();

void moveForwardOneGrid(int rpm) {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  double distanceTicks = 0.95 * 9.5 * ticksPerCM;
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  int m1setSpeed = 100;
  int m2setSpeed = 96;

  MotorPID M1pid = {m1setSpeed, 0, 0, 0.111};//0.110
  MotorPID M2pid = {m2setSpeed , 0, 0, 0.1084};//0.109

  //Check using right side sensor if need to calibrate
  calibrateBeforeMoveForward();

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);
  setTicks(0, 0);

  md.setSpeeds(m1setSpeed, m2setSpeed);
  while (1)
  {
    //Serial.print(sqWidthToRPM(squareWidth_M1));
    //Serial.print(" ");
    //Serial.println(sqWidthToRPM(squareWidth_M2));

    noInterrupts();
    currentTicksM1 = M1ticks;
    currentTicksM2 = M2ticks;
    interrupts();

    if (currentTicksM1 >= distanceTicks || currentTicksM2 >= distanceTicks)
    {
      md.setBrakes(400, 400);
      break;
    }

    tuneEntryTime = micros();//Can try removing interval for single grid movement
    interval = tuneEntryTime - tuneExitTime;
    if (interval >= 5000)
    {
      tuneMotors(rpm, &M1pid, &M2pid);
      tuneExitTime = micros();
    }

  }//end of while

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
}

void moveForwardBeta(int rpm, double distance) {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  double distanceTicks = 1.05 * distance * ticksPerCM;
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  int m1setSpeed = 108;
  int m2setSpeed = 92;

  //Check using right side sensor if need to calibrate
  calibrateBeforeMoveForward();

  MotorPID M1pid = {m1setSpeed, 0, 0, 0.109};//0.1=>50
  MotorPID M2pid = {m2setSpeed, 0, 0, 0.1165};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms //was 0.125

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);
  setTicks(0, 0);

  md.setSpeeds(m1setSpeed, m2setSpeed);

  while (1)
  {
    noInterrupts();
    currentTicksM1 = M1ticks;
    currentTicksM2 = M2ticks;
    interrupts();

    if (currentTicksM1 >= distanceTicks || currentTicksM2 >= distanceTicks)
    {
      md.setBrakes(400, 400);
      break;
    }

    tuneEntryTime = micros();
    interval = tuneEntryTime - tuneExitTime;
    if (interval >= 5000)
    {
      tuneMotors(rpm, &M1pid, &M2pid);

      //Gradual breaking
      /*
        if(currentTicksM1 < 0.7*distanceTicks){
        //tuneM1(rpm, &M1pid);
        //tuneM2(rpm, &M2pid);
        tuneMotors(rpm, &M1pid, &M2pid);
        }else if(currentTicksM1 < 0.85*distanceTicks){
        //tuneM1(rpm*0.75, &M1pid);
        //tuneM2(rpm*0.75, &M2pid);
        tuneMotors(rpm*0.75, &M1pid, &M2pid);
        }else{
        //tuneM1(rpm*0.4, &M1pid);
        //tuneM2(rpm*0.4, &M2pid);
        tuneMotors(rpm*0.4, &M1pid, &M2pid);
        }
      */
      tuneExitTime = micros();
    }

  }//end of while

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
}

void moveForwardOneGridBeta() {

  unsigned long tuneEntryTime = 0;
  unsigned long tuneExitTime = 0;
  unsigned long interval = 0;
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  
  int m1setSpeed = 310;
  int m2setSpeed = 300;
  int tuneSpeedM1 = 0;
  int tuneSpeedM2 = 0;

  breakTicks = 0.97 * 9.5 * ticksPerCM;
  
  //Check using right side sensor if need to calibrate
  calibrateBeforeMoveForward();

  MotorPID M2 = {m2setSpeed , 0, 0, 0.5}; 
  enableInterrupt( e1a, risingM1Ticks, RISING);
  enableInterrupt( e2b, risingM2Ticks, RISING);
  md.setSpeeds(m1setSpeed, m2setSpeed);
     
      while(!movementDone)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 

            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
            tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.07)*(M2.currentErr - M2.prevErr1);
            if(!movementDone)
              OCR1B = tuneSpeedM2;
            
            M2.prevTuneSpeed = tuneSpeedM2;
            M2.prevErr1 = M2.currentErr;
            tuneExitTime = micros();
      
            //Serial.print("M2.currentErr");
            //Serial.println(M2.currentErr);
          }//end of if
          
        }// end of while   
      Serial.print("breakTicksM2: ");
      Serial.println(M2ticks);
      Serial.print("breakTicksM1: ");
      Serial.println(M1ticks);

  disableInterrupt(e1a);
  disableInterrupt(e2b);
  breakTicks = 0;
  movementDone = false;
  setTicks(0, 0);
  setSqWidth(0, 0);
}
