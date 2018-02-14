#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"
#include <Wire.h>



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
signed long ticksPerCM = singlerevticks/wheelDiameter;

//------------Interrupt declarations------------
signed long startCaptureTime = 0.55 * 1000000;       // 0.1 seconds for free standing

volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;
volatile signed long ticks = 0;

volatile int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;
volatile signed long entry_time_M2 = 0;

void risingM1()
{
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
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
  md.init();
  moveForward(80, 80);
  
}

void loop() {

}

void moveForward(int rpm, int distance){

   signed long tStart = micros();
   signed long tuneEntryTime = 0;
   signed long tuneExitTime = 0;
   signed long interval = 0;
   signed long distanceTicks = distance * ticksPerCM;
   int pidStartRPM = rpm;  //rpm/2
   boolean startPID = false;
   
   PCintPort::attachInterrupt(e1a, &risingM1, RISING);
   PCintPort::attachInterrupt(e2b, &risingM2, RISING);

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
    md.setM1Speed(241.298);
    delay(1.8);
    md.setM2Speed(284.546);
    
   
   while(ticks < distanceTicks){//18432
      
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

    md.setM1Brake(300);
    md.setM2Brake(300);
    delay(500);
    ticks = 0;
    md.setM2Speed(-180);//342.698
    md.setM1Speed(-160);//296.216

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
   
signed long sqWidthToRPM(int sqWidth){

  if(sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
  signed long sqwavesOneS = 1000000/sqWidth;//1/(sqWidth/1000000)
  signed long sqwavesOneM = sqwavesOneS*60;
  signed long revPerMin = sqwavesOneM/sqwavesPerRev;

  return revPerMin;
 
  }

int getAvgSqWidth(int arr[]){

  signed long sum = 0;
  int avg = 0;
   
  for(int i = 0; i < 10; i++){  
   sum += arr[i];
  }

  avg = (sum / 10);
  Serial.print("Avg: ");
  Serial.println(avg);
  return avg;
 
  
  
  }
