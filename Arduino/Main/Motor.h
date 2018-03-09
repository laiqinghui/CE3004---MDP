#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//------------PID constants and declarations--------
#define sampleInterval 0.005

//Motor 1(Right) constant
#define td1 0.00496 
#define ts1 0.06667 //Orig: 0.09667, tune: 0.08667 //Higher denotes more gain on correction
#define k1 0.35093

//Motor 2(Left) constant
#define td2 0.00264
#define ts2 0.0355 //Orig: 0.05827, tune: 0.09827 
#define k2 0.31323

//Motor 1(Right) PID parameters
signed long kc1 = (1.2*ts1)/(k1*td1);
signed long ti1 = 2*td1;
signed long kp1 = kc1;
signed long ki1 = kc1/ti1;
signed long kd1 = kc1*td1;

signed long k1_1 = kp1+ki1+kd1;
signed long k2_1 = (-kp1-2)*kd1;
signed long k3_1 = kd1;

//Motor 2(left) PID parameters
signed long kc2 = (1.2*ts2)/(k2*td2);
signed long ti2 = 2*td2;
signed long kp2 = kc2;
signed long ki2 = kc2/ti2;
signed long kd2 = kc2*td2;

signed long k1_2 = kp2+ki2+kd2;
signed long k2_2 = (-kp2-2)*kd2;
signed long k3_2 = kd2;


signed long currentErr_M1 = 0;
signed long prevErr1_M1 = 0;
signed long prevErr2_M1 = 0;

signed long currentErr_M2 = 0;
signed long prevErr1_M2 = 0;
signed long prevErr2_M2 = 0;


//------------Other constants and declrations----------
#define Pi 3.1416
#define singlerevticks 2248.86

signed long wheelDiameter = 6*Pi;
signed long ticksPerCM = 119.305728441; //singlerevticks/wheelDiameter;


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
  M1Ticks +=4;
}

//ISR for Motor2(Left) encoder
void risingM2()
{
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = micros();
}



void setM1Ticks(int ticks){
  
  M1Ticks = ticks;
  
}

void setSqWidth(int M1, int M2){
  
  squareWidth_M1 = M1;
  squareWidth_M2 = M2;
  
  
}

double getTurnTicksOffsetAmt(int turnDegree)
{
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
  
  switch(turnDegree){
    
    case 0 ... 90:
      return 0.90;
    case 360 ... 450:
      return 0.90;
    default:
      return 0.85;
    
    }
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
    double cir = Pi * 16.4; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6    
    int amount = abs(cir * (turnDegree/360.0) * ticksPerCM);//int to ignored decimal value //* getTurnTicksOffsetAmt(turnDegree)
    int ticks = 0;
    boolean brakeSet = false;
    
    Serial.print("Target count: ");
    Serial.println(amount);
    Serial.print("Offset amt: ");
    Serial.println(getTurnTicksOffsetAmt(turnDegree));
  
  
    PCintPort::attachInterrupt(e1a, &risingM1, RISING); 
    setM1Ticks(0); 
    
    
	md.setSpeeds(-270 * dir, 300 * dir);
	while(abs(M1Ticks) < amount - 500)
    {
    }
	md.setSpeeds(-158.921 * dir, 197.318 * dir);//80 RPM
	while(abs(M1Ticks) < amount)
    {
    }

    md.setBrakes(400,400);
    Serial.print("Current amt: ");
    Serial.println(ticks);
    
  setSqWidth(0,0);//Reset sqWidth
  setM1Ticks(0);// Reset M1Ticks
    PCintPort::detachInterrupt(e1a); 
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

void tuneM1(int desiredRPM){

  
  double tuneSpeed;
  double currentRPM = sqWidthToRPM(squareWidth_M1);
  Serial.print("M1 Current RPM: ");
  Serial.println(currentRPM);
  
  
  currentErr_M1 =  desiredRPM - currentRPM;
  tuneSpeed = currentRPM + k1_1*currentErr_M1 + k2_1*prevErr1_M1 + k3_1*prevErr2_M1;
  /*
  Serial.print("currentErr_M1 ");
  Serial.println(currentErr_M1);
  Serial.print("prevErr1_M1 ");
  Serial.println(prevErr1_M1);
  Serial.print("prevErr2_M1 ");
  Serial.println(prevErr2_M1);
  Serial.print("M1 tuneSpeed ");
  Serial.println(tuneSpeed);
  Serial.println();
  */
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
  /*
  Serial.print("M2 tuneSpeed ");
  Serial.println(tuneSpeed);
  */
  md.setM2Speed(tuneSpeed);

  prevErr2_M2 = prevErr1_M2;
  prevErr1_M2 = currentErr_M2;

  
  }  
   
  
void moveForward(int rpm, int distance){

   signed long tStart = micros();
   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = rpm;  
   boolean startPID = false;
   
   setM1Ticks(0);// Reset M1Ticks
   PCintPort::attachInterrupt(e1a, &risingM1, RISING);
   PCintPort::attachInterrupt(e2b, &risingM2, RISING);
   Serial.println("Moving forward...\nInterrupt attached\nSetting speed now...");

  md.setSpeeds(235.298, 284.546);
    
   
   while(M1Ticks < distanceTicks){//18432
      
      if(!startPID ){
        
        if( sqWidthToRPM(squareWidth_M1) >= pidStartRPM ||  sqWidthToRPM(squareWidth_M2) >= pidStartRPM ){ //(micros() - tStart) > startCaptureTime
          
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

    //md.setM1Brake(400);
    //md.setM2Brake(400);
  for(int i = 50; i<=400; i++){
    md.setM1Brake(i);
    md.setM2Brake(i);
  }
  /*
  //For reverse
    delay(500);
    setM1Ticks(0);
    md.setM2Speed(-180);//342.698
    md.setM1Speed(-160);//296.216
    while(M1Ticks < distanceTicks);
    md.setM1Brake(250);
    md.setM2Brake(300);
    */
  setSqWidth(0,0);//Reset sqWidth
  setM1Ticks(0);// Reset M1Ticks
  PCintPort::detachInterrupt(e1a);
  PCintPort::detachInterrupt(e2b);
   
  }
