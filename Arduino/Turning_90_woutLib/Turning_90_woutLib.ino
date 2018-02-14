#include <Wire.h>
#include <PinChangeInt.h>
#include "DualVNH5019MotorShield.h"

#define Pi 3.1416

//------------Interrupt declarations------------
volatile unsigned long M1Ticks = 0; //Global

void risingM1()
{
  M1Ticks +=4;
}

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13


DualVNH5019MotorShield md;

void setup() {
  
  Serial.begin(115200);
  
  md.init();
  
  turn(-1, 90);
}

void loop()
{
  

  
}

void turn(int dir, int turnDegree)
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
    
    double cir = Pi * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = 2248.86/(6*Pi); //cm to counts for wheel
    int amount = abs(cir * (turnDegree/360.0) * cmToCounts) * getTurnTicksOffsetAmt(turnDegree);//int to ignored decimal value
    unsigned long ticks = 0;
    boolean brakeSet = false;
    
    
    
    PCintPort::attachInterrupt(e1a, &risingM1, RISING);  
    Serial.print("Target count: ");
    Serial.println(amount);
    Serial.print("Offset amt: ");
    Serial.println(getTurnTicksOffsetAmt(turnDegree));
    md.setSpeeds(-158.921 * dir, 197.318 * dir);//80 RPM
    
    while(ticks < amount)
    {
      ticks = abs(M1Ticks);
      Serial.print("M1 current En count: ");
      Serial.println(ticks);
      /*
      if(ticks > amount*0.85 && !brakeSet){ //Activate brakes at 85 percent of target ticks
        md.setBrakes(400,400);
        brakeSet = true;
        Serial.println("Brakes activated ");
        }
        
      */
    }

    md.setBrakes(400,400);
    
}

double getTurnTicksOffsetAmt(int turnDegree)
{
  switch(turnDegree){
    
    case 0 ... 90:
      return 0.85;
    case 360 ... 450:
      return 0.90;
    default:
      return 0.85;
    
    }
}
