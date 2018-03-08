#include <EnableInterrupt.h>
#include "DualVNH5019MotorShield.h"
#include "Sensors.h"

DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//------------PID Structs------------
struct MotorPID {
  float prevTuneSpeed;
  float currentErr;
  float prevErr1;
  float gain;
};

void tuneM1Negative(int desiredRPM, MotorPID *M1);
void tuneM2Negative(int desiredRPM, MotorPID *M2);

//------------Other constants and declrations----------
#define singlerevticks 562.215

signed long wheelDiameter = 18.849556; //Diameter is 6cm
signed long ticksPerCM = singlerevticks/wheelDiameter;


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
void risingM1(){
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks++;
}

void risingM1Ticks(){
  M1ticks++;
  }
 

//ISR for Motor2(Left) encoder
void risingM2(){
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
  M2ticks++;
}

void setTicks(int M1, int M2){
	M1ticks = M1;
  M2ticks = M2;
}

void setSqWidth(int M1, int M2){
	squareWidth_M1 = M1;
	squareWidth_M2 = M2;
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
  
  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);

  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);

 
  }

void tuneM2(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain*M2->currentErr + (M2->gain/0.05)*(M2->currentErr - M2->prevErr1);

  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;
  
 
  }

void tuneM1Negative(int desiredRPM, MotorPID *M1){


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);
  
  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);
  Serial.println(tuneSpeed);

  md.setM1Speed(tuneSpeed);
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);

 
  }

void tuneM2Negative(int desiredRPM, MotorPID *M2){
  
  
  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);
  
  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain*M2->currentErr + (M2->gain/0.05)*(M2->currentErr - M2->prevErr1);

  md.setM2Speed(tuneSpeed);
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;
  
 
  }
 
void moveForward(int rpm, double distance, boolean pidOn){

   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   double offset = 1;
   if(distance == 9.5)
    offset = 0.92;
   
   double distanceTicks = offset  * distance * ticksPerCM;//Delibrate trimming
   unsigned long currentTicksM1 = 0;
   unsigned long currentTicksM2 = 0;
    
    MotorPID M1pid = {255, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {310 , 0, 0, 0.132};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    //md.setSpeeds(100, 100);
    md.setSpeeds(255,310);
    

    Serial.print("Target Ticks: ");
    Serial.println(distanceTicks);

    while(1){
      
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
      
      if(pidOn){
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ 

          tuneM1(rpm, &M1pid);
          tuneM2(rpm, &M2pid);
          
          if(distance >= 19){
            if(currentTicksM1 < 0.7*distanceTicks){
              tuneM1(rpm, &M1pid);
              tuneM2(rpm, &M2pid);
            } else if(currentTicksM1 < 0.85*distanceTicks){
              tuneM1(rpm*0.75, &M1pid);
              tuneM2(rpm*0.75, &M2pid);
            } else{
              tuneM1(rpm*0.5, &M1pid);
              tuneM2(rpm*0.5, &M2pid);
              }
        }  
        
          tuneExitTime = micros();
        }
      }
      noInterrupts();
      currentTicksM1 = M1ticks;
      currentTicksM2 = M2ticks;
      interrupts();
      
      if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks){
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
            md.setM1Brake(400);
            md.setM2Brake(400);
            Serial.print("M1BreakTicks: ");
            Serial.println(currentTicksM1);
            Serial.print("M2BreakTicks: ");
            Serial.println(currentTicksM2);
            break;
            }
          }
          
      
      
        break;
      }       
     
      
    }//end of while
      
      //md.setBrakes(400,400);
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
}  

//-1 is left turn and 1 is right turn
double getTurnAmountPID(int dir, int turnDegree){
    if(dir == 1)
    {
      double degree90 = 50; //cir is 51.8
      double degree180 = 50.8; //cir is 53.1
      if(turnDegree < 90)
      {
        return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
      }
      else
      {
        double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
        double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
        
        return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
    else
    {
      double degree90 = 47; //cir is 47.6
      double degree180 = 48.7; //cir is 49.65
      if(turnDegree < 90)
      {
        Serial.println(abs(degree90 * (turnDegree/360.0) * ticksPerCM));
        return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
      }
      else
      {
        double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
        double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
        
        return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
}

double getTurnTicksOffsetAmt(int turnDegree)
{
  switch(turnDegree){
    
    case 0 ... 90:
      return 0.95;
    case 91 ... 180:
      return 1;  
    case 360 ... 450:
      return 0.90;
    default:
      return 0.85;
    
    }
}

void turnTemp1(int dir, int turnDegree)
{
    //1 is right, -1 is left
    
    /*
     * Offset Amount(%) for turning angles:
     * 0 - 90: 85%
     * 360 - 450: 
     * 450 - 540: 
     * 540 - 630: 
     * 630 - 720: 
     * 720 - 810: 
     * 810 - 900:
     * 900 - 990:
     * 990 - 1080:
     */
    
    double cir = 3.142 * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = singlerevticks/(6*3.142); //cm to counts for wheel
    int amount = abs(cir * (turnDegree/360.0) * cmToCounts) * getTurnTicksOffsetAmt(turnDegree);//int to ignored decimal value
    unsigned long ticks = 0;
    boolean brakeSet = false;
    
    
    
    enableInterrupt( e1a, risingM1, RISING);
    Serial.print("Target count: ");
    Serial.println(amount);
    Serial.print("Offset amt: ");
    Serial.println(getTurnTicksOffsetAmt(turnDegree));
    md.setSpeeds(-158.921 * dir, 197.318 * dir);//80 RPM
    
    while(1)
    {
      noInterrupts();
      ticks = M1ticks;
      interrupts();
      if(ticks > amount){
        break;
        }
      
      /*
      if(ticks > amount*0.85 && !brakeSet){ //Activate brakes at 85 percent of target ticks
        md.setBrakes(400,400);
        brakeSet = true;
        Serial.println("Brakes activated ");
        }
        
      */
    }
    setTicks(0,0);
    setSqWidth(0,0);
    disableInterrupt(e1a);
    md.setBrakes(400,400);
    
}



//-1 is left turn and 1 is right turn
void turnPID(int dir, int turnDegree)
{
    
    setTicks(0,0);
    setSqWidth(0,0);
    
    //1 is right, -1 is left 
    int distanceTicks = getTurnAmountPID(dir, turnDegree);    
    int rpm = 100;
    /*
     * Different Speed Values
     * md.setSpeeds(-221 * dir, 250 * dir);
     * md.setSpeeds(-168 * dir, 200 * dir);
     * md.setSpeeds(-269 * dir, 314 * dir);
    */ 
   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0; 
   unsigned long currentTicks = 0;
    
    
    enableInterrupt( e1a, risingM1, RISING);
    enableInterrupt( e2b, risingM2, RISING);

    //md.setSpeeds(100, 100);
    //Dir = 1
     
    if(dir == 1)
	{
    MotorPID M1pid = {-255, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {310, 0, 0, 0.132 };//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms
		md.setSpeeds(-255, 310);
		while(1)
		{
      Serial.println("In While");
			tuneEntryTime = micros();
			interval = tuneEntryTime - tuneExitTime;
			if(interval >= 5000)
			{ 
			  tuneM1Negative(rpm, &M1pid);
			  tuneM2(rpm, &M2pid);
			}  
			tuneExitTime = micros();
			noInterrupts();
			currentTicks = M1ticks;
			interrupts();
		  
			if(currentTicks>=distanceTicks)
			{
				break;
			}
		}//end of while
		md.setBrakes(400,400);
	}
	else
	{
    MotorPID M1pid = {269, 0, 0, 0.1};//0.1=>50
    MotorPID M2pid = {-314, 0, 0, 0.132 };//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms  
		md.setSpeeds(269, -314);
		while(1)
		{
      //Serial.println("In TurnPID");
			tuneEntryTime = micros();
			interval = tuneEntryTime - tuneExitTime;
			if(interval >= 5000)
			{ 
			  tuneM1(rpm, &M1pid);
			  tuneM2Negative(rpm, &M2pid);
			}  
			tuneExitTime = micros();
			noInterrupts();
			currentTicks = M1ticks;
			interrupts();
			if(currentTicks>=distanceTicks)
			{
				break;
			}
		}//end of while
		md.setBrakes(400,400);
	}
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
}

//-1 is left turn and 1 is right turn
double getTurnAmount(int dir, int turnDegree){
    if(dir == 1)
    {
  		double degree90 = 50; //cir is 51.8
  		double degree180 = 50.8; //cir is 52.9
  		if(turnDegree < 90)
  		{
  			return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
  		}
  		else
  		{
  			double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
  			double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
  		  
  			return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
    else
    {
		  double degree90 = 47; //cir is 47.6
		  double degree180 = 48.73; //cir is 49.65
      if(turnDegree < 90)
      {
        Serial.println(abs(degree90 * (turnDegree/360.0) * ticksPerCM));
        return abs(degree90 * (turnDegree/360.0) * ticksPerCM);
      }
      else
      {
        double closenessTo90 = ((turnDegree-90)/90.0)*degree180;
        double closenessTo180 = ((180 - turnDegree)/90.0)*degree90;
        
        return abs((closenessTo90 + closenessTo180) * (turnDegree/360.0) * ticksPerCM);
      }
    }
}

//-1 is left turn and 1 is right turn
void turnTemp(int dir, int turnDegree){
    //1 is right, -1 is left 
    int amount = getTurnAmount(dir, turnDegree);
    int ticks = 0;
    int previousRead = 0;
    int currentValue = 0;
    
    /*
     * Different Speed Values
     * md.setSpeeds(-221 * dir, 250 * dir);
     * md.setSpeeds(-168 * dir, 200 * dir);
     * md.setSpeeds(-269 * dir, 314 * dir);
    */
    noInterrupts();
    if(dir == 1)
    {
      md.setSpeeds(-269, 314);
      while(true)
      {
        currentValue = (PINB>>5) & B001; 
        if(currentValue && !previousRead)
        {
          ticks++;
          if(ticks == amount)
          {
            md.setBrakes(400,400);
            break;        
          }
        } 
        previousRead = currentValue;
      }
    }
    else
    {
      md.setSpeeds(269, -314);
      while(true)
      {
        currentValue = (PIND>>3) & B001;
        if(currentValue && !previousRead)
        {
          ticks++;
          if(ticks == amount)
          {
            md.setBrakes(400,400);
            break;        
          }
        } 
        previousRead = currentValue;
      }
    }
    interrupts();
}

