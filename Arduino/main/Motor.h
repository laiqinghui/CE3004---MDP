#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"

DualVNH5019MotorShield md(2, 4, 6, A0, 7, 8, 12, A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//------------PID Structs------------
struct MotorPID {
  int prevTuneSpeed;
  float currentErr;
  float prevErr1;
  float gain;
};

void tuneM1Negative(int desiredRPM, MotorPID *M1);
void tuneM2Negative(int desiredRPM, MotorPID *M2);

//------------Other constants and declrations----------
#define singlerevticks 562.215

signed long wheelDiameter = 18.849556; //Diameter is 6cm
signed long ticksPerCM = singlerevticks / wheelDiameter;


//------------Interrupt declarations------------
volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;
volatile unsigned long M1ticks = 0;

volatile int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;
volatile signed long entry_time_M2 = 0;
volatile unsigned long M2ticks = 0;

//ISR for Motor1(Right) encoder
void risingM1() {
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks++;
}

void risingM1Ticks() {
  M1ticks++;
}

void risingM2Ticks() {
  M2ticks++;
}

//ISR for Motor2(Left) encoder
void risingM2() {
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
  M2ticks++;
}

void setTicks(int M1, int M2) {
  M1ticks = M1;
  M2ticks = M2;
}

void setSqWidth(int M1, int M2) {
  squareWidth_M1 = M1;
  squareWidth_M2 = M2;
}

signed long sqWidthToRPM(int sqWidth) {

  if (sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
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

  md.setM1Speed(tuneSpeed);
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

  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;


}

void tuneM1Negative(int desiredRPM, MotorPID *M1) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);

  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed - M1->gain * M1->currentErr - (M1->gain / 0.05) * (M1->currentErr - M1->prevErr1);

  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);


}

void tuneM2Negative(int desiredRPM, MotorPID *M2) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);

  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed - M2->gain * M2->currentErr - (M2->gain / 0.05) * (M2->currentErr - M2->prevErr1);

  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;


}

void moveForward(int rpm, double distance, boolean pidOn) {

  signed long tuneEntryTime = 0;
  signed long tuneExitTime = 0;
  signed long interval = 0;
  double offset = 1;
  if (distance == 9.5)
    offset = 0.95;

  double distanceTicks = offset  * distance * ticksPerCM;//Delibrate trimming
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  MotorPID M1pid = {260, 0, 0, 0.1};//0.1=>50
  MotorPID M2pid = {300 , 0, 0, 0.115};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);

  //md.setSpeeds(100, 100);
  md.setSpeeds(260, 300);


  Serial.print("Target Ticks: ");
  Serial.println(distanceTicks);

  while (1) {

    //Serial.print(sqWidthToRPM(squareWidth_M1));
    //Serial.print(" ");
    //Serial.println(sqWidthToRPM(squareWidth_M2));

    if (pidOn) {
      tuneEntryTime = micros();
      interval = tuneEntryTime - tuneExitTime;
      if (interval >= 5000) {

        tuneM1(rpm, &M1pid);
        tuneM2(rpm, &M2pid);

        if (distance >= 19) {
          if (currentTicksM1 < 0.7 * distanceTicks) {
            tuneM1(rpm, &M1pid);
            tuneM2(rpm, &M2pid);
          } else if (currentTicksM1 < 0.85 * distanceTicks) {
            tuneM1(rpm * 0.75, &M1pid);
            tuneM2(rpm * 0.75, &M2pid);
          } else {
            tuneM1(rpm * 0.5, &M1pid);
            tuneM2(rpm * 0.5, &M2pid);
          }
        }

        tuneExitTime = micros();
      }
    }
    noInterrupts();
    currentTicksM1 = M1ticks;
    currentTicksM2 = M2ticks;
    interrupts();

    if (currentTicksM1 >= distanceTicks || currentTicksM2 >= distanceTicks) {
      md.setBrakes(400, 400);
      /*
        if(currentTicksM1>=distanceTicks)
        md.setM1Brake(400);
        else md.setM2Brake(400);
        Serial.print("M1BreakTicks: ");
        Serial.println(currentTicksM1);
        Serial.print("M2BreakTicks: ");
        Serial.println(currentTicksM2);
        while(1){
        noInterrupts();
        currentTicksM1 = M1ticks;
        currentTicksM2 = M2ticks;
        interrupts();
        if(currentTicksM1>=distanceTicks && currentTicksM2>=distanceTicks ){
          md.setBrakes(400, 400);
          Serial.print("M1BreakTicks: ");
          Serial.println(currentTicksM1);
          Serial.print("M2BreakTicks: ");
          Serial.println(currentTicksM2);
          break;
          }
        }


      */
      break;
    }  //end of if


  }//end of while

  //md.setBrakes(400,400);
  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
}

//-1 is left turn and 1 is right turn
double getTurnAmount(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    double degree90Offset = 1.02; //cir is 51.8
    double degree180Offset = 1.01; //cir is 52.9
    if (turnDegree <= 90)
    {
      return abs(49 * 0.25 * ticksPerCM) * degree90Offset;
    }
    else
    {

      return abs(51.6 * 0.5 * ticksPerCM) * degree180Offset;
    }
  }
  //Left Turn
  else
  {
    double degree90Offset = 0.98; //cir is 47.6
    //double degree180 = 48.73; //cir is 49.65
    return abs(48.2 * (turnDegree / 360.0) * ticksPerCM) * degree90Offset;
  }
}

//-1 is left turn and 1 is right turn
void turnTest(int dir, int turnDegree) {
  //1 is right, -1 is left
  int amount = getTurnAmount(dir, turnDegree);
  int ticks = 0;
  int previousRead = 0;
  int currentValue = 0;

  /*
     Different Speed Values
     md.setSpeeds(-221 * dir, 250 * dir);
     md.setSpeeds(-168 * dir, 200 * dir);
     md.setSpeeds(-269 * dir, 314 * dir);
  */
  noInterrupts();
  if (dir == 1)
  {
    md.setSpeeds(-269, 314);
    while (true)
    {
      currentValue = (PINB >> 5) & B001;
      if (currentValue && !previousRead)
      {
        ticks++;
        if (ticks == amount)
        {
          md.setBrakes(400, 400);
          break;
        }
      }
      previousRead = currentValue;
    }
  }
  else
  {
    md.setSpeeds(269, -314);
    while (true)
    {
      currentValue = (PIND >> 3) & B001;
      if (currentValue && !previousRead)
      {
        ticks++;
        if (ticks == amount)
        {
          md.setBrakes(400, 400);
          break;
        }
      }
      previousRead = currentValue;
    }
  }
  interrupts();
  
}

double getTurnTest(int dir, int turnDegree) {
  //Right Turn
  if (dir == 1)
  {
    //180
    if (turnDegree == 180)
    {
	  return abs(47 * 0.5 * ticksPerCM);
      
    }
    //90
    else
    {
		return abs(45 * (turnDegree / 360.0) * ticksPerCM);
      
    }
  }
  //Left Turn
  else
  {
    return abs(44 * (turnDegree / 360.0) * ticksPerCM);
  }
}

void turnPID(int dir, int degree){
	sideWall[0] = 0;
	sideWall[1] = 0;
	sideWall[2] = 0;
	
	int amount = getTurnTest(dir, degree);
	
	int total = 0;
	int currentTicksM1 = 0;
	int currentTicksM2 = 0;
  int difference = 0;
	
	if(dir == 1)
	{
		int m1Speed = -302;
		int m2Speed = 330;

		enableInterrupt( e1a, risingM1Ticks, RISING);
		enableInterrupt( e2b, risingM2Ticks, RISING);
		md.setSpeeds(m1Speed, m2Speed);
    setTicks(0, 0);

		while(currentTicksM1 < amount)
		{
      
			noInterrupts();
			currentTicksM1 = M1ticks;
			currentTicksM2 = M2ticks;
			interrupts();
     
			if(currentTicksM1 < currentTicksM2)
			{

			  m2Speed = m2Speed - 1;
			  OCR1B = m2Speed;
			}
			else if(currentTicksM1 > currentTicksM2)
			{
        m2Speed = m2Speed + 1;
			  OCR1B = m2Speed;
			}
		}
	}
	else
	{
		int m1Speed = 302;
		int m2Speed = -330;

		enableInterrupt( e1a, risingM1Ticks, RISING);
		enableInterrupt( e2b, risingM2Ticks, RISING);
		md.setSpeeds(m1Speed, m2Speed);

		while(currentTicksM1 < amount)
		{
			noInterrupts();
			currentTicksM1 = M1ticks;
			currentTicksM2 = M2ticks;
			interrupts();
		
			if(currentTicksM1 < currentTicksM2)
			{
			  m2Speed = m2Speed + 1;
			  OCR1B = m2Speed;
			}
			else if(currentTicksM1 > currentTicksM2)
			{
			  m2Speed = m2Speed - 1;
			  OCR1B = m2Speed;
			}
		}
	}
	md.setBrakes(400, 400);

	disableInterrupt(e1a);
	disableInterrupt(e2b);
	setTicks(0, 0);
}
