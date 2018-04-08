#include "Constants.h"
#include <math.h>

//------------PID Structs------------
struct MotorPID {
  int prevTuneSpeed;
  signed long currentErr;
  signed long prevErr1;
  double gain;
};

//------------Interrupt declarations------------
volatile long int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;

volatile long int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;


//----------------------------------------------------------------PID----------------------------------------------------------------
//ISR for Motor1(Right) encoder
void risingM1() {
  
  int entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  //squareWidth_M1 = squareWidth_M1 + squareWidth_M1;// MULTIPLY BY TWO TO GET PULSEWIDTH 
  prev_time_M1 = entry_time_M1;
  
  //squareWidth_M1 = micros() - prev_time_M1;
  //prev_time_M1 = prev_time_M1 + squareWidth_M1;
}

void dummy()
{
}

//ISR for Motor2(Left) encoder
void risingM2() {
  int entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  //squareWidth_M2 = squareWidth_M2 + squareWidth_M2;// MULTIPLY BY TWO TO GET PULSEWIDTH 
  prev_time_M2 = entry_time_M2;
}

void setSqWidth(int M1, int M2) {
  squareWidth_M1 = M1;
  squareWidth_M2 = M2;
}

signed long sqWidthToRPM(int sqWidth) {

  if (sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.215 ;
  signed long sqwavesOneS = 1000000 / sqWidth; //1/(sqWidth/1000000)
  signed long sqwavesOneM = sqwavesOneS * 60;
  signed long revPerMin = sqwavesOneM / sqwavesPerRev;

  return revPerMin;

}

void tuneM1(int desiredRPM, MotorPID *M1) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);

  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain * M1->currentErr + (M1->gain / 0.05) * (M1->currentErr - M1->prevErr1);

  OCR1A = tuneSpeed;
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);


}

void tuneM2(int desiredRPM, MotorPID *M2) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);

  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain * M2->currentErr + (M2->gain / 0.05) * (M2->currentErr - M2->prevErr1);

  OCR1B = tuneSpeed;
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;


}

void tuneMotors(int desiredRPM, MotorPID *M1, MotorPID *M2) {
	long int currentSquareWidth_M1 = 0;
	long int currentSquareWidth_M2 = 0;
	
	noInterrupts();
	currentSquareWidth_M1 = squareWidth_M1;
	currentSquareWidth_M2 = squareWidth_M2;
	interrupts();
	
	
	int tuneSpeedM1 = 0;
	int tuneSpeedM2 = 0;
	
	double currentM1RPM = sqWidthToRPM(currentSquareWidth_M1);
	double currentM2RPM = sqWidthToRPM(currentSquareWidth_M2);
  /*
  Serial.println("currentM1RPM");
  Serial.println(currentM1RPM);
  Serial.println("currentM2RPM");
  Serial.println(currentM2RPM);
  */
	M1->currentErr =  desiredRPM - currentM1RPM;
	tuneSpeedM1 = M1->prevTuneSpeed + M1->gain * M1->currentErr + (M1->gain / 0.05) * (M1->currentErr - M1->prevErr1);
	M2->currentErr =  desiredRPM - currentM2RPM;
	tuneSpeedM2 = M2->prevTuneSpeed + M2->gain * M2->currentErr + (M2->gain / 0.05) * (M2->currentErr - M2->prevErr1);
	
	noInterrupts();
	if(!movementDone)
	{
		OCR1A = tuneSpeedM1;
		OCR1B = tuneSpeedM2;
	}
	interrupts();

	M1->prevTuneSpeed = tuneSpeedM1;
	M1->prevErr1 = M1->currentErr;
	M2->prevTuneSpeed = tuneSpeedM2;
	M2->prevErr1 = M2->currentErr;
}
